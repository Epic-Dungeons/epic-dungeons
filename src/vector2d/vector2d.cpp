#include "vector2d.h"
#include <cmath>
using namespace std;
Vector2d::Vector2d(const float &x, const float &y) : x(x), y(y) {};

Vector2d &Vector2d::operator=(const Vector2d &other) {
    const_cast<float &>(x) = other.x;
    const_cast<float &>(y) = other.y;
    return *this;
}

const Vector2d Vector2d::operator+(const Vector2d &other) const {
    return Vector2d(x + other.x, y + other.y);
}

const Vector2d Vector2d::operator-(const Vector2d &other) const {
    return Vector2d(x - other.x, y - other.y);
}

const Vector2d Vector2d::operator-() const {
    return Vector2d(-x, -y);
}

const Vector2d Vector2d::operator*(const float &b) const {
    return Vector2d(x * b, y * b);
}

const Vector2d Vector2d::operator/(const float &b) const {
    return Vector2d(x / b, y / b);
}

const Vector2d Vector2d::round() const {
    return Vector2d(roundf(x), roundf(y));
}

const float Vector2d::length_squared() const {
    return x * x + y * y;
}

const float Vector2d::length() const {
    return sqrt(length_squared());
}

const Vector2d Vector2d::normalized() const {
    return *this / length();
}

const int Vector2d::manhattan_length() const {
    return abs(x) + abs(y);
}

const sf::Vector2f Vector2d::toSF() const {
    return sf::Vector2f(x, y);
}

const string Vector2d::toString() const {
    return "(" + to_string(x) + ", " + to_string(y) + ")";
}

const Vector2d &Vector2d::operator+=(const Vector2d &other) {
    const_cast<float &>(x) += other.x;
    const_cast<float &>(y) += other.y;
    return *this;
}

const Vector2d &Vector2d::operator-=(const Vector2d &other) {
    const_cast<float &>(x) -= other.x;
    const_cast<float &>(y) -= other.y;
    return *this;
}

const Vector2d &Vector2d::operator*=(const float &b) {
    const_cast<float &>(x) *= b;
    const_cast<float &>(y) *= b;
    return *this;
}

const Vector2d &Vector2d::operator/=(const float &b) {
    const_cast<float &>(x) /= b;
    const_cast<float &>(y) /= b;
    return *this;
}