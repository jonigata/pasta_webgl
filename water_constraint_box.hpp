// 2018/01/09 Naoyuki Hirayama

/*!
	@file	  water_constraint_box.hpp
	@brief	  <ŠT—v>

	<à–¾>
*/

#ifndef WATER_CONSTRAINT_BOX_HPP_
#define WATER_CONSTRAINT_BOX_HPP_

#include "water.hpp"

// box constraint
class WaterConstraint_Box : public IConstraint {
    const float BOX_WIDTH = 400.0f; // 40cm‚Ì” 
    const float BOX_HEIGHT = 400.0f; // 40cm‚Ì” 

public:
    WaterConstraint_Box() { case_offset_ = Vector2(56, 56); }

    Vector2 apply(const Vector2& vv) {
        Vector2 v = vv;
        float minx = 0 + case_offset_.x;
        if (v.x < minx) {
            v.x = minx - (minx - v.x) * 0.80f;
        }
        float maxx = BOX_WIDTH + case_offset_.x;
        if (maxx < v.x) {
            v.x = maxx + (v.x - maxx) * 0.80f;
        }
        float maxy = BOX_HEIGHT + case_offset_.y;
        if (maxy < v.y) {
            v.y = maxy + (v.y - maxy) * 0.80f;
        }
        return v;
    }

    Vector2 case_offset_;
};

#endif // WATER_CONSTRAINT_BOX_HPP_
