//
// Created by Voric and tobisdev on 11/08/2024.
//
#ifndef CAR_H
#define CAR_H

#include <SFML/Graphics.hpp>

struct carData {
    std::string name;
    sf::Texture carTexture;
    float MaxSpeed;
    float Handling;
    float Acceleration;
    float weight;
    float power;
    float torque;
    std::string driveType;
    sf::Texture logoTexture;
};

class Car {
public:
    Car();
    void resetVelocity();
    void resetAngularAcceleration();
    void resetRotationAngle();
    void update(float dt);
    void render(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;
    void applyData(carData& data);
    float getRotationAngle() const;
    sf::Vector2f getCurrentPosition() const;
    void setPreviousPosition(const sf::Vector2f& position);
    void setCurrentPosition(const sf::Vector2f& position);
    void setRotationAngle(float angle);
    sf::Sprite& getCarSprite();

    // New setters and getters
    void setAcceleration(float value);
    void setAngularAcceleration(float value);
    float getAccelerationConstant() const;
    float getAngularAccelerationConstant() const;

private:
    sf::Sprite carSprite;
    sf::Vector2f current_position;
    sf::Vector2f previous_position;
    sf::Vector2f velocity;
    float rotation_angle;
    float angular_velocity;
    float acceleration;
    float angular_acceleration;
    float angular_damping;
    float friction_coefficient;
    float acceleration_constant;
    float angular_acceleration_constant;
    float max_speed;

    // Other car data
    float maxSpeedValue;
    float handlingValue;
    float accelerationValue;
};

#endif // CAR_H
