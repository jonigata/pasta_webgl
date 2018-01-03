// 2008/12/25 Naoyuki Hirayama

#include "water.hpp"
#include "piece.hpp"
// #include "zw/dprintf.hpp"
// #include "performance_counter.hpp"

const float DOT_SIZE			   = 0.01f;

class WaterRenderer {
public:
    WaterRenderer() {}

    void operator()(
        int     id,
        const Vector2& pos,
        float    mass,
        float    density_plain,
        float    density_balance_corrected,
        float    density_repulsive_corrected,
        float    boundariness,
        const SPH_Traits_Portable_2D::load_type& load) {
        
        float cc = boundariness < 1.0f ? 0.08f : 1.0f;
        Color c = Color {{ cc, 0.0f, 1.0f, 0.25f }};

        Vector2 sx(DOT_SIZE, 0);
        Vector2 sy(0, DOT_SIZE);
        Vector2 sxy(DOT_SIZE, DOT_SIZE);
        
        Vector2 posud(pos.x, 512.0f - pos.y);
        Vector2 pos2 = posud / 512 - sxy * 0.5f;

        piece_.add_vi(pos2, c);
        piece_.add_vi(pos2 + sx, c);
        piece_.add_vi(pos2 + sy, c);
        piece_.add_vi(pos2 + sy, c);
        piece_.add_vi(pos2 + sx, c);
        piece_.add_vi(pos2 + sxy, c);
    }

    void build() {
        piece_.build();
    }

    void render() {
        piece_.render();
    }

private:
    Piece piece_;
};

/*============================================================================
 *
 * class Water 
 *
 * 
 *
 *==========================================================================*/
//<<<<<<<<<< Water

//****************************************************************
// constructor
Water::Water() {
    constraint_ = nullptr;

    sph_.initialize(
        SEARCH_RADIUS,
        VISCOSITY,
        DUMPING,
        Vector2(0, GRAVITY),
        IDEAL_DENSITY,
        PRESSURE_BALANCE_COEFFICIENT,
        PRESSURE_REPULSIVE_COEFFICIENT);

    for (int y = 0 ; y < VCOUNT ; y++) {
        for (int x = 0 ; x < HCOUNT ; x++) {
            sph_.add_particle(
                Vector2(256.0f, 256.0f)+ 
                Vector2(float(-HCOUNT/2+x), float(-VCOUNT/2+y))*
                INITIAL_DISTANCE,
                MASS);
        }
    }
}

//****************************************************************
// render
void Water::render() {
    WaterRenderer r;
    sph_.foreach(std::ref(r));
    r.build();
    r.render();
}

//****************************************************************
// update
void Water::update() {
    // PerformanceCounter pc(true);
    sph_.update(0.01f);
    // pc.print("update");

    if (constraint_) {
        sph_.constraint(
            [this](const Vector2& v) { return constraint_->apply(v); });
        // pc.print("constraint");
    }
}
//>>>>>>>>>> Water

