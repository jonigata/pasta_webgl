// 2018/01/04 Naoyuki Hirayama

/*!
	@file	  water_in_bottle.hpp
	@brief	  <概要>

	<説明>
*/

#ifndef WATER_IN_BOTTLE_HPP_
#define WATER_IN_BOTTLE_HPP_

#include "water.hpp"
#include "water_constraint_box.hpp"
#include "water_constraint_tm.hpp"
#include <memory>

class WaterInBottle {
public:
    WaterInBottle() : tm_(0, 0, 512, 512) {}

    void build(gci::Document<Vector2> doc) {
        // constraint_ = std::make_unique<WaterConstraint_Box>();
        
        gci2pathview::gci_to_trapezoidal_map(doc, tm_);
        tmm_.init(tm_);
        constraint_ =
            std::make_unique<WaterConstraint_TrapezoidalMap>(doc, tmm_);

        water_.set_constraint(constraint_.get());
    }

    void update() {
        water_.update();
    }

    void render() {
        water_.render();
    }

private:
    Water water_;
    Vector2 case_offset_;
    std::unique_ptr<IConstraint> constraint_;

private:
    // Constraint constraint_;
    
private:
    // trapezoidal map constraint
    TrapezoidalMap<float, SegmentProperty> tm_;
    TrapezoidalMapMachine<float, SegmentProperty> tmm_;

};


#endif // WATER_IN_BOTTLE_HPP_
