// 2008/10/27 Naoyuki Hirayama

/*!
	@file	  water.hpp
	@brief	  <äTóv>

	<ê‡ñæ>
*/

#ifndef WATER_HPP_
#define WATER_HPP_

#include "sph.hpp"
#include "sph_portable_traits.hpp"

#if 1
const int HCOUNT    = 40;
const int VCOUNT    = 20;
#else
const int HCOUNT    = 2;
const int VCOUNT    = 2;
#endif

#if 0
const float INITIAL_DISTANCE	   = 10.0f;	// 1cm(10mm)ä‘äuÇÃäiéqÇçÏÇÈ
const float DT                     = 0.01f;	// 100ÉtÉåÅ[ÉÄ/s
const float SEARCH_RADIUS          = 50.0f;	// 5cm(50mm)ÇÃåüçıîÕàÕ
const float VISCOSITY              = 1.0f;
const float DUMPING                = 0.99f;
const float GRAVITY                = 9.8f;	// mm/(s*s)
const float MASS                   = 1.0f;
const float IDEAL_DENSITY          = 1.0f; // 10.0f;
const float PRESSURE_BALANCE_COEFFICIENT    = 0.0f;
const float PRESSURE_REPULSIVE_COEFFICIENT  = 5.0f;
#else
const float INITIAL_DISTANCE	   = 10.0f;	// 1cm(10mm)ä‘äuÇÃäiéqÇçÏÇÈ
const float DT                     = 0.01f;	// 100ÉtÉåÅ[ÉÄ/s
const float SEARCH_RADIUS          = 50.0f;	// 5cm(50mm)ÇÃåüçıîÕàÕ
const float VISCOSITY              = 1.0f;
const float DUMPING                = 0.99f;
const float GRAVITY                = 9800.0f;	// mm/(s*s)
const float MASS                   = 1.0f;
const float IDEAL_DENSITY          = 10.0f;
const float PRESSURE_BALANCE_COEFFICIENT    = 5.0f;
const float PRESSURE_REPULSIVE_COEFFICIENT  = 5.0f;
#endif

class IConstraint {
public:
    virtual ~IConstraint() {}

    virtual Vector2 apply(const Vector2&) = 0;
};


class Water {
public:
    Water();
    ~Water() {}

    void render();

    void click(Vector2& p) {}
    void update();

    void  set_viscosity(float v) { sph_.set_viscosity(v); }
    float  get_viscosity() { return sph_.get_viscosity(); }
    void  set_dumping(float d) { sph_.set_dumping(d); }
    float  get_dumping() { return sph_.get_dumping(); }
    void  set_ideal_density(float d) { sph_.set_ideal_density(d); }
    float  get_ideal_density() { return sph_.get_ideal_density(); }

    void  set_constraint(IConstraint* constraint) { constraint_ = constraint; }

private:
    sph::sph<SPH_Traits_Portable_2D> sph_;
    IConstraint*     constraint_;

};

#endif // WATER_HPP_
