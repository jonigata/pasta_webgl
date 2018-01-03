// 2018/01/01 Naoyuki Hirayama

/*!
	@file	  vector2d.hpp
	@brief	  <概要>

	<説明>
*/

#ifndef VECTOR2D_HPP_
#define VECTOR2D_HPP_

#include <cmath>

struct Vector2 {
    float x;
    float y;

    Vector2(){}
    Vector2(float ax, float ay)
        : x(ax), y(ay) {}

    void assign(float ax, float ay) {
        x = ax; y = ay;
    }

};

inline Vector2 operator+(const Vector2& lhs, const Vector2& rhs) {
    Vector2 v;
    v.x = lhs.x + rhs.x;
    v.y = lhs.y + rhs.y;
    return v;
}

inline Vector2 operator*(const Vector2& lhs, float rhs) {
    Vector2 v;
    v.x = lhs.x * rhs;
    v.y = lhs.y * rhs;
    return v;
}

inline Vector2 operator/(const Vector2& lhs, float rhs) {
    return lhs * (1.0f / rhs);
}

inline Vector2 operator-(const Vector2& v) {
    Vector2 r;
    r.x = -v.x;
    r.y = -v.y;
    return r;
}

inline Vector2 operator-(const Vector2& lhs, const Vector2& rhs) {
    Vector2 v;
    v.x = lhs.x - rhs.x;
    v.y = lhs.y - rhs.y;
    return v;
}

inline Vector2& operator+=(Vector2& lhs, const Vector2& rhs) {
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    return lhs;
}

inline Vector2& operator-=(Vector2& lhs, const Vector2& rhs) {
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    return lhs;
}

inline Vector2& operator*=(Vector2& lhs, float rhs) {
    lhs.x *= rhs;
    lhs.y *= rhs;
    return lhs;
}

inline Vector2& operator/=(Vector2& lhs, float rhs) {
    return operator*=(lhs, 1.0f / rhs);
}

inline float length_sq(const Vector2& v) {
    return v.x * v.x + v.y * v.y;
}

inline float length(const Vector2& v) {
    return sqrtf(length_sq(v));
}

#endif // VECTOR2D_HPP_
