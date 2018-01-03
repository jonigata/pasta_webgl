// 2018/01/04 Naoyuki Hirayama

/*!
	@file	  water_in_bottle.hpp
	@brief	  <概要>

	<説明>
*/

#ifndef WATER_IN_BOTTLE_HPP_
#define WATER_IN_BOTTLE_HPP_

#include "water.hpp"
#include "trapezoidal_map.hpp"

class WaterInBottle {
private:
    class Constraint : public IConstraint {
        const float BOX_WIDTH = 400.0f; // 40cmの箱
        const float BOX_HEIGHT = 400.0f; // 40cmの箱

    public:
        Constraint() { case_offset_ = Vector2(0, 0); }

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

public:
    WaterInBottle() {
        water_.set_constraint(&constraint_);
    }

    void update() {
        water_.update();
    }

    void render() {
        water_.render();
    }

private:
    Water water_;
    Constraint constraint_;
    
    Vector2 case_offset_;

};


#endif // WATER_IN_BOTTLE_HPP_
