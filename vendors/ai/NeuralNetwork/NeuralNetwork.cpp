//
// Created by Tobias on 02.10.2024.
//

#include "NeuralNetwork.h"

NeuralNetwork::NeuralNetwork(std::vector<int> &topology, std::vector<Utility::Activations> &activations, int n) {
    if(activations.size() != topology.size() - 1){
        std::cerr << "Sizes do not match!" << "\n";
        return;
    }

    _activations = activations;
    for (int i = 1; i < topology.size(); ++i) {
        int &currentNeurons = topology[i];
        int &previousNeurons = topology[i - 1];

        _weights.emplace_back(currentNeurons, previousNeurons, n);
        _biases.emplace_back(currentNeurons, 1, n);
    }
}

NeuralNetwork::NeuralNetwork(std::vector<int> &topology, std::vector<Utility::Activations> &activations, float min, float max, bool uniform, int n) {
    if(activations.size() != topology.size() - 1){
        std::cerr << "Sizes do not match!" << "\n";
        return;
    }

    _activations = activations;
    for (int i = 1; i < topology.size(); ++i) {
        int &currentNeurons = topology[i];
        int &previousNeurons = topology[i - 1];

        if(uniform){
            _weights.emplace_back(af::randu(currentNeurons, previousNeurons, n) * (max - min) + min);
            _biases.emplace_back(af::randu(currentNeurons, 1, n) * (max - min) + min);
        }else{
            _weights.emplace_back(af::randn(currentNeurons, previousNeurons, n) * (max - min) + min);
            _biases.emplace_back(af::randn(currentNeurons, 1, n) * (max - min) + min);
        }
    }
}

af::array NeuralNetwork::feed_forward(af::array &input) {
    af::array value = input;

    if (_weights.empty()) {
        std::cerr << "The network does not possess any layers!" << "\n";
        return value;
    }

    if (input.dims()[0] != _weights[0].dims()[1] || input.dims()[2] != _weights[0].dims()[2]) {
        std::cerr << "The input dimension must match the first layer's weight dimensions!" << "\n";
        return value;
    }

    // The batches are stored in the 4th dimension because the 3rd dimension is occupied by the networks
    // Get the batch size from the input
    dim_t batchSize = value.dims()[3];

    for (int i = 0; i < _weights.size(); ++i) {

        af::array weights = af::tile(_weights[i], 1, 1, 1, batchSize);
        af::array biases = af::tile(_biases[i], 1, 1, 1, batchSize);

        // z = activation(weights * inputs + biases)
        value = af::matmul(weights, value) + biases;
        value = Utility::calculate_activation(value, _activations[i]);
    }

    return value;
}

af::array NeuralNetwork::feed_forward(std::vector<float> &input){
    af::array in = Utility::vectorToArray(input);
    return feed_forward(in);
}

af::array NeuralNetwork::feed_forward_single(af::array &input, int index){
    af::array value = input;

    if (_weights.empty()) {
        std::cerr << "The network does not possess any layers!" << "\n";
        return value;
    }

    if (input.dims()[0] != _weights[0].dims()[1]) {
        std::cerr << "The input dimension must match the first layer's weight dimensions!" << "\n";
        return value;
    }

    af::array lookup = af::constant(index, 1, 1, 1, 1, u32);
    // Get the batch size from the input
    dim_t batchSize = value.dims()[2];

    for (int i = 0; i < _weights.size(); ++i) {
        af::array weightSlice = af::lookup(_weights[i], lookup, 2);
        af::array biasSlice = af::lookup(_biases[i], lookup, 2);

        weightSlice = af::tile(weightSlice, 1, 1, batchSize);
        biasSlice = af::tile(biasSlice, 1, 1, batchSize);

        // z = activation(weights * inputs + biases)
        value = af::matmul(weightSlice, value) + biasSlice;
        value = Utility::calculate_activation(value, _activations[i]);
    }

    return value;
}

af::array NeuralNetwork::feed_forward_single(std::vector<float> &input, int index){
    af::array in = Utility::vectorToArray(input);
    return feed_forward_single(in, index);
}

void NeuralNetwork::breed(std::vector<float> &fitness, int winners, float min, float max, bool uniform) {
    if (_weights.empty()) {
        std::cerr << "The network does not possess any layers!" << "\n";
        return;
    }

    unsigned int numNetworks = _weights[0].dims()[2];

    if(winners > numNetworks){
        std::cerr << "The number of winners cannot be higher than the number of networks!\n";
        return;
    }

    // Buffer for child networks
    std::vector<af::array> weights;
    std::vector<af::array> biases;

    // Already apply the mutation values at start
    for (int i = 0; i < _weights.size(); ++i) {
        if(uniform){
            weights.emplace_back(af::randu(_weights[i].dims()) * (max - min) + min);
            biases.emplace_back(af::randu(_biases[i].dims()) * (max - min) + min);
        }else{
            weights.emplace_back(af::randn(_weights[i].dims()) * (max - min) + min);
            biases.emplace_back(af::randn(_biases[i].dims()) * (max - min) + min);
        }
    }

    // Find the best neural networks
    auto selectedNetworks = Utility::find_top_n(fitness, winners);

    // Copy the winners into the children to preserve them
    af::array selectedIdxArray(1, selectedNetworks.size(), selectedNetworks.data());

    for (int layer = 0; layer < _weights.size(); ++layer) {
        af::array selectedWeights = af::lookup(_weights[layer], selectedIdxArray.as(u32), 2);
        af::array selectedBiases = af::lookup(_biases[layer], selectedIdxArray.as(u32), 2);

        // Assign the selected weights and biases to the first numSelectedNetworks positions
        af::seq destSeq(0, selectedNetworks.size() - 1);

        weights[layer](af::span, af::span, destSeq) = selectedWeights;
        biases[layer](af::span, af::span, destSeq) = selectedBiases;
    }

    // Decide the breeding pairs
    unsigned int numPairs = numNetworks - winners;
    std::vector<unsigned int> n1Vec(numPairs), n2Vec(numPairs);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, selectedNetworks.size() - 1);

    for(int i = 0; i < numPairs; ++i){
        int idx1 = dis(gen);
        int idx2 = dis(gen);

        n1Vec[i] = idx1;
        n2Vec[i] = idx2;
    }
    af::array n1Array(1, numPairs, n1Vec.data());
    af::array n2Array(1, numPairs, n2Vec.data());

    // Cross the values of the networks
    for (int layer = 0; layer < _weights.size(); ++layer) {
        af::dim4 wDims = _weights[layer].dims();
        af::dim4 bDims = _biases[layer].dims();

        // Generate masks for all network pairs
        af::array wMasks = af::randu(wDims[0], wDims[1], numPairs) > 0.5f;
        af::array bMasks = af::randu(bDims[0], bDims[1], numPairs) > 0.5f;

        // Extract parent weights and biases using the lookup function along the third dimension
        af::array parent1Weights = af::lookup(_weights[layer], n1Array.as(u32), 2);
        af::array parent2Weights = af::lookup(_weights[layer], n2Array.as(u32), 2);

        af::array parent1Biases = af::lookup(_biases[layer], n1Array.as(u32), 2);
        af::array parent2Biases = af::lookup(_biases[layer], n2Array.as(u32), 2);

        // Perform crossover using masks
        af::array newWeights = parent1Weights * wMasks + parent2Weights * (1 - wMasks);
        af::array newBiases = parent1Biases * bMasks + parent2Biases * (1 - bMasks);

        // Assign the new weights and biases to the appropriate slices
        af::seq destSeq(winners, winners + numPairs - 1);

        weights[layer](af::span, af::span, destSeq) += newWeights;
        biases[layer](af::span, af::span, destSeq) += newBiases;
    }

    // Copy the children into the networks
    for (int layer = 0; layer < _weights.size(); ++layer) {
        _weights[layer] = weights[layer];
        _biases[layer] = biases[layer];
    }
}

void NeuralNetwork::breed(af::array &fitness, int winners, float min, float max, bool uniform){
    auto in = Utility::arrayToVector(fitness);
    breed(in, winners, min, max, uniform);
}

int NeuralNetwork::networks() {
    if (_weights.empty()) {
        std::cerr << "The network does not possess any layers!" << "\n";
        return -1;
    }
    return _weights[0].dims()[2];
}

int NeuralNetwork::size() {
    return (int)_weights.size() + 1;
}

size_t NeuralNetwork::bytes() {
    if (_weights.empty()) {
        std::cerr << "The network does not possess any layers!" << "\n";
        return -1;
    }
    size_t size = 0;
    for (int i = 0; i < _weights.size(); ++i) {
        size += _biases[i].bytes() + _weights[i].bytes();
    }
    return size;
}

std::vector<int> NeuralNetwork::topology() {
    std::vector<int> output;

    for (int i = 0; i < _weights.size(); ++i) {
        int currentNeurons = (int)_weights[i].dims()[0];
        int previousNeurons = (int)_weights[i].dims()[1];

        if(i == 0){
            output.emplace_back(previousNeurons);
        }

        output.emplace_back(currentNeurons);
    }

    return output;
}

bool NeuralNetwork::save(std::string path, int n)
{
    // Ensure that n does not exceed the actual number of networks
    int numNetworks = this->networks(); // the total networks in the 3rd dimension
    if (n > numNetworks) {
        n = numNetworks;
    }

    // Prepare JSON
    nlohmann::json j;
    j["topology"] = this->topology();

    std::vector<int> activationVec(_activations.size());
    for (size_t i = 0; i < _activations.size(); i++) {
        activationVec[i] = static_cast<int>(_activations[i]);
    }
    j["activations"]  = activationVec;
    j["num_networks"] = numNetworks;

    nlohmann::json layersJson = nlohmann::json::array();
    for (size_t i = 0; i < _weights.size(); i++) {
        // Slice out only [0..n-1] in the 3rd dimension
        af::array wSub = _weights[i](af::span, af::span, af::seq(0, n - 1), af::span);
        af::array bSub = _biases[i](af::span, af::span, af::seq(0, n - 1), af::span);

        // Retrieve shapes for the partial slices
        af::dim4 wShape = wSub.dims();
        af::dim4 bShape = bSub.dims();

        // Convert the partial arrays to std::vector<float>
        std::vector<float> wData = Utility::arrayToVector(wSub);
        std::vector<float> bData = Utility::arrayToVector(bSub);

        // Build JSON layer entry
        nlohmann::json layerJson;
        layerJson["weights_shape"] = { (int)wShape[0], (int)wShape[1], (int)wShape[2], (int)wShape[3] };
        layerJson["biases_shape"]  = { (int)bShape[0], (int)bShape[1], (int)bShape[2], (int)bShape[3] };

        layerJson["weights_data"]  = wData;
        layerJson["biases_data"]   = bData;

        layersJson.push_back(layerJson);
    }
    j["layers"] = layersJson;

    // Write to file
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << path << "\n";
        return false;
    }
    file << j.dump(4);
    file.close();
    return true;
}

bool NeuralNetwork::load(std::string path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for reading: " << path << "\n";
        return false;
    }

    nlohmann::json j;
    try {
        file >> j;
    } catch (const nlohmann::json::exception &e) {
        std::cerr << "JSON parse error: " << e.what() << "\n";
        file.close();
        return false;
    }
    file.close();

    auto top = j["topology"].get<std::vector<int>>();
    auto actVec = j["activations"].get<std::vector<int>>();
    int net = j["num_networks"].get<int>();

    // Convert activations
    std::vector<Utility::Activations> loadedActivations(actVec.size());
    for (size_t i = 0; i < actVec.size(); i++) {
        loadedActivations[i] = static_cast<Utility::Activations>(actVec[i]);
    }

    int networks = _weights[0].dims(2);

    _weights.clear();
    _biases.clear();
    _activations = loadedActivations;

    // Parse each layer from the JSON
    auto layersJson = j["layers"];
    for (auto &layer : layersJson) {
        // Shapes
        auto wShape = layer["weights_shape"].get<std::vector<int>>();
        auto bShape = layer["biases_shape"].get<std::vector<int>>();

        // Data
        std::vector<float> wData = layer["weights_data"].get<std::vector<float>>();
        std::vector<float> bData = layer["biases_data"].get<std::vector<float>>();

        // Construct the ArrayFire arrays
        af::array wArr(wShape[0], wShape[1], wShape[2], wShape[3], wData.data());
        af::array bArr(bShape[0], bShape[1], bShape[2], bShape[3], bData.data());

        // Check if 'net' is greater than what was actually saved in this layer
        int savedNetworks = wArr.dims()[2]; // number of networks in the 3rd dimension
        if (net > savedNetworks && savedNetworks > 0) {
            // Calculate how many full repeats we need, plus any leftover
            int fullRepeats = (networks) / savedNetworks;
            int leftover    = networks % savedNetworks;

            // Repeat the entire block 'fullRepeats' times
            af::array wRepeated = af::tile(wArr, 1, 1, fullRepeats, 1);
            af::array bRepeated = af::tile(bArr, 1, 1, fullRepeats, 1);

            // If there's a remainder, append a partial slice
            if (leftover > 0) {
                wRepeated = af::join(
                        2,
                        wRepeated,
                        wArr(af::span, af::span, af::seq(0, leftover - 1), af::span)
                );
                bRepeated = af::join(
                        2,
                        bRepeated,
                        bArr(af::span, af::span, af::seq(0, leftover - 1), af::span)
                );
            }
            wArr = wRepeated;
            bArr = bRepeated;
        }
        // If net <= savedNetworks, or savedNetworks == 0, we do nothing special.

        _weights.push_back(wArr);
        _biases.push_back(bArr);
    }

    return true;
}

NeuralNetwork::NeuralNetwork(std::string path) {
    load(path);
}
