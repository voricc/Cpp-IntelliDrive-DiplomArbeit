//
// Created by Voric and tobisdev on 11/08/2024.
//

#include "../include/Car.h"
#include <iostream>
#include <cmath>

constexpr float PI = 3.14159265f;

constexpr float MAX_ACCELERATION_CONSTANT = 1000.0f; // Units: pixels per second squared
constexpr float MAX_ANGULAR_ACCELERATION_CONSTANT = 200.0f; // Units: degrees per second squared
constexpr float MAX_SPEED = 900.0f; // Units: pixels per second

Car::Car()
    : current_position(0.0f, 0.0f),
      previous_position(0.0f, 0.0f),
      velocity(0.0f, 0.0f),
      rotation_angle(0.0f),
      angular_velocity(0.0f),
      acceleration(0.0f),
      angular_acceleration(0.0f),
      angular_damping(0.1f),
      friction_coefficient(0.30f), // Adjust between 0 (no friction) and 1 (full stop instantly)
      acceleration_constant(10.1f),
      angular_acceleration_constant(0.1f),
      max_speed(10.0f),
      maxSpeedValue(5.0f),
      handlingValue(5.0f),
      accelerationValue(5.0f)
{

}

void Car::handleInput() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        acceleration = acceleration_constant;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        acceleration = -acceleration_constant;
    } else {
        acceleration = 0.0f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        angular_acceleration = -angular_acceleration_constant;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        angular_acceleration = angular_acceleration_constant;
    } else {
        angular_acceleration = 0.0f;
    }
}

void Car::resetVelocity() {
    velocity.x = 0.0f;
    velocity.y = 0.0f;
}

void Car::resetAngularAcceleration() {
    angular_acceleration = 0.0f;
    acceleration = 0.0f;
    angular_velocity = 0.0f;
    angular_damping = 0.0f;
}

void Car::update(float dt) {
    float radian_angle = rotation_angle * (PI / 180.0f);

    sf::Vector2f forward_direction(sinf(radian_angle), -cosf(radian_angle));

    velocity += forward_direction * acceleration * dt;

    velocity -= velocity * friction_coefficient * dt;

    float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    if (speed > max_speed) {
        velocity = (velocity / speed) * max_speed;
    }

    current_position += velocity * dt;

    angular_velocity += angular_acceleration * dt;

    angular_velocity -= angular_velocity * angular_damping * dt;

    rotation_angle += angular_velocity * dt;

    if (rotation_angle >= 360.0f) {
        rotation_angle -= 360.0f;
    } else if (rotation_angle < 0.0f) {
        rotation_angle += 360.0f;
    }

    carSprite.setPosition(current_position);
    carSprite.setRotation(rotation_angle);

    previous_position = current_position;
}

void Car::resetRotationAngle() {
    rotation_angle = 0.0f;
}

void Car::render(sf::RenderWindow& window) {
    window.draw(carSprite);
    carSprite.setScale(0.35f, 0.35f);
}

sf::FloatRect Car::getBounds() const {
    return carSprite.getGlobalBounds();
}

void Car::applyData(carData &data) {
    carSprite = {};
    carSprite.setTexture(data.carTexture);

    maxSpeedValue = static_cast<float>(data.MaxSpeed);
    handlingValue = static_cast<float>(data.Handling);
    accelerationValue = static_cast<float>(data.Acceleration);

    acceleration_constant = accelerationValue * (MAX_ACCELERATION_CONSTANT / 10.0f);
    angular_acceleration_constant = handlingValue * (MAX_ANGULAR_ACCELERATION_CONSTANT / 10.0f);
    max_speed = maxSpeedValue * (MAX_SPEED / 10.0f);
}

float Car::getRotationAngle() const {
    return rotation_angle;
}

sf::Vector2f Car::getCurrentPosition() const {
    return current_position;
}

void Car::setPreviousPosition(const sf::Vector2f& position) {
    previous_position = position;
}

void Car::setCurrentPosition(const sf::Vector2f& position) {
    current_position = position;
}

sf::Sprite& Car::getCarSprite() {
    return carSprite;
}
