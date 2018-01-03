// 2018/01/01 Naoyuki Hirayama

/*!
	@file	  vector2d.hpp
	@brief	  <ŠT—v>

	<à–¾>
*/

#ifndef VECTOR4D_HPP_
#define VECTOR4D_HPP_

struct Vector4 {
    float x;
    float y;
    float z;
    float w;

    Vector4(){}
    Vector4(float ax, float ay, float az, float aw)
        : x(ax), y(ay), z(az), w(aw) {}

    void assign(float ax, float ay, float az, float aw) {
        x = ax; y = ay; z = az; w = aw;
    }

};

#endif // VECTOR4D_HPP_
