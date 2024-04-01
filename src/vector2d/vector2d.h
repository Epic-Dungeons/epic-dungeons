#pragma once
#include <SFML/System/Vector2.hpp>
#include <string>

class Vector2d {
public:
    Vector2d(const float &x, const float &y);

    template<typename T>
    Vector2d(const sf::Vector2<T> &vector) : x(vector.x), y(vector.y) {};

    Vector2d &operator=(const Vector2d &other);
    const Vector2d operator+(const Vector2d &other) const;
    const Vector2d operator-(const Vector2d &other) const;
    const Vector2d operator-() const;
    const Vector2d operator*(const float &b) const;
    const Vector2d operator/(const float &b) const;
    const Vector2d &operator+=(const Vector2d &other);
    const Vector2d &operator-=(const Vector2d &other);
    const Vector2d &operator*=(const float &b);
    const Vector2d &operator/=(const float &b);
    const Vector2d round() const;
    const float length_squared() const;
    const float length() const;
    const Vector2d normalized() const;
    const int manhattan_length() const;

    const std::string toString() const;

    const sf::Vector2f toSF() const;

    const float x, y;
};
