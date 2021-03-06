// 2008/12/24 Naoyuki Hirayama

/*!
	@file	  sph.hpp
	@brief	  <概要>

	<説明>
*/

#ifndef SPH_HPP_
#define SPH_HPP_


// N次元対応SPH

// #include "performance_counter.hpp"

// 長さ mm
// 質量 g
// 時間 s

#include <cstdlib>
#include <cmath>
#include <vector>
#include <cstdio>

namespace sph {

namespace detail {

template <class Traits>
struct Particle {
    typedef typename Traits::real_type  real_type;
    typedef typename Traits::vector_type vector_type;
    typedef typename Traits::load_type  load_type;

    Particle* next;
    int   id;

    vector_type new_position;
    vector_type old_position;
    real_type mass;
    real_type pressure_balance;
    real_type pressure_repulsive;
    real_type density0;
    real_type density_plain;
    real_type density_balance;
    real_type density_repulsive;
    real_type density_balance_numerator;
    real_type density_balance_denominator;
    real_type density_balance_corrected;
    real_type density_repulsive_numerator;
    real_type density_repulsive_denominator;
    real_type density_repulsive_corrected;
    real_type i_density_plain;
    real_type i_density_balance;
    real_type i_density_repulsive;
    real_type boundariness; // boundary if <1.0f
    //vector_type move;
    load_type load;
};

template <class Traits>
struct Pair {
    typedef typename Traits::real_type  real_type;
    typedef typename Traits::vector_type vector_type;
    typedef typename Traits::load_type  load_type;
    typedef Particle<Traits> particle_type;

    particle_type* car;
    particle_type* cdr;
    vector_type diff;
    real_type length_sq;
    real_type length;
};

template <class Traits>
class HashTable {
public:
    template <class V>
    HashTable(V& v) {
        memset(table_, 0, sizeof(table_));

        for (typename V::iterator i = v.begin(); i != v.end(); ++i) {
            Particle<Traits>& pi = *i;

            int coord[Traits::DIMENSION];
            Traits::make_coords(coord, pi.new_position);
            size_t h = Traits::hash(coord, TABLE_SIZE);
            pi.next = table_[h];
            table_[h] = &pi;
        }
    }
    ~HashTable(){ }

    Particle<Traits>* find(const int a[]) {
        return table_[Traits::hash(a, TABLE_SIZE)];
    }

private:
    enum { TABLE_SIZE = 4997 };
    Particle<Traits>* table_[TABLE_SIZE];
};
	
template <class Traits, int N>
struct when_n {
    typedef typename Traits::real_type      real_type;
    typedef typename Traits::vector_type    vector_type;
    typedef typename Traits::load_type      load_type;
    typedef Particle<Traits>                particle_type;
    typedef Pair<Traits>                    pair_type;
    typedef HashTable<Traits>               hashtable_type;

    static void exec(
        particle_type& pi,
        hashtable_type& ht,
        const int coords[2][Traits::DIMENSION], // 2 means min, max
        int c[Traits::DIMENSION],
        std::vector<pair_type>& pairs) {

        for (int i = coords[0][Traits::DIMENSION-N] ;
             i <= coords[1][Traits::DIMENSION-N] ; i++) {
            c[Traits::DIMENSION-N] = i;
            when_n<Traits, N-1>::exec(pi, ht, coords, c, pairs);
        }
    }
};

template <class Traits>
struct when_n<Traits, 0> {
    typedef typename Traits::real_type      real_type;
    typedef typename Traits::vector_type    vector_type;
    typedef typename Traits::load_type      load_type;
    typedef Particle<Traits>                particle_type;
    typedef Pair<Traits>                    pair_type;
    typedef HashTable<Traits>               hashtable_type;

    static void exec(
        particle_type& pi,
        hashtable_type& ht,
        const int coords[2][Traits::DIMENSION], // 2 means min, max
        int c[Traits::DIMENSION],
        std::vector<pair_type>& pairs) {
        
        for (particle_type* p = ht.find(c); p ; p = p->next) {
            particle_type& pj = *p;
            if (&pj <= &pi) { continue; }

            vector_type v = pj.new_position - pi.new_position;
            real_type length_sq = Traits::length_sq(v);
            if (real_type(1.0) <= length_sq) { continue; }

            pair_type pair;
            pair.car = &pi;
            pair.cdr = &pj;
            pair.diff = v;
            pair.length_sq = length_sq;
            pair.length = sqrt(length_sq);
            pairs.push_back(pair);
        }
    }
};


template <class Traits>
using update_pairs_n = when_n<Traits, Traits::DIMENSION>;

}

template <class Traits>
class sph {
private:
    typedef typename Traits::real_type  real_type;
    typedef typename Traits::vector_type vector_type;
    typedef typename Traits::load_type  load_type;
    typedef detail::Particle<Traits> particle_type;
    typedef detail::Pair<Traits> pair_type;

    static real_type kernelc() {
        return
            real_type(315.0) /
            (real_type(64.0) * real_type(3.1415926535));
    }
    static real_type kernel2(real_type length) {
        real_type x = real_type(1.0) - length;
        return x * x;
    }
    static real_type kernel3(real_type length) {
        real_type x = real_type(1.0) - length;
        return x * x * x;
    }

public:
    sph() {}
    ~sph() {}

    void initialize(
        real_type search_radius,
        real_type viscosity,
        real_type dumping,
        vector_type gravity,
        real_type ideal_density,
        real_type pressure_balance_coefficient,
        real_type pressure_repulsive_coefficient) {
        C_ = 0;

        // radiusが1.0になるようにパラメータを正規化
        //(9乘しているところとかあるので)
        src_search_radius_ = search_radius;
        viscosity_   = viscosity;
        dumping_   = dumping;
        gravity_   = gravity / search_radius;
        ideal_density_  = ideal_density;
        pressure_balance_coefficient_ = pressure_balance_coefficient;
        pressure_repulsive_coefficient_ = pressure_repulsive_coefficient;

        case_offset_ = Traits::zero_vector();
    }

    void add_particle(const vector_type& v, real_type mass) {
        particle_type p;
        p.new_position = v / src_search_radius_;
        p.old_position = p.new_position;
        p.mass = mass;
        p.density0 = ideal_density_;
        p.density_balance = ideal_density_;
        p.density_repulsive = ideal_density_;
        p.i_density_balance = real_type(1)/ p.density_balance;
        //p.move = Traits::zero_vector();
        particles_.push_back(p);
    }

    int pick(const vector_type& p, real_type range) {
        real_type maxlen =(std::numeric_limits<real_type>::max)();  

        int id = -1;
        for (size_t i = 0 ; i <particles_.size(); i++) {
            vector_type pos = particles_[i].new_position;

            vector_type diff = pos - p;
            real_type len = Traits::length_square(diff);
            if (len <maxlen) {
                maxlen = len;
                id = int(i);
            }
        }

        if (maxlen <= range) {
            return id;
        }
        return -1;
    }

    template <class F>
    void foreach(F f) {
        for (size_t i = 0 ; i <particles_.size(); i++) {
            particle_type& p = particles_[i];
            f(
                p.id,
                p.new_position * src_search_radius_,
                p.mass,
                p.density_plain,
                p.density_balance_corrected,
                p.density_repulsive_corrected,
                p.boundariness,
                p.load);
        }
    }
	
    template <class F>
    void constraint(F f) {
        real_type i_src_search_radius = real_type(1)/ src_search_radius_;

        for (size_t i = 0 ; i <particles_.size(); i++) {
            particle_type& p = particles_[i];
            p.new_position =
                f(p.new_position * src_search_radius_)
                * i_src_search_radius;
        }
    }
	
    void update(real_type dt) {
        C_ = 0;

        // real_type idt = real_type(1.0)/ dt;
        real_type dt2 = square(dt);

        // verlet integration
        for (size_t i = 0 ; i <particles_.size(); i++) {
            particle_type& p = particles_[i];

#if 0
            // use previous position to compute next velocity
            vector_type vdt = p.new_position - p.old_position;
            vector_type v = vdt * idt;

            // save previous position
            p.old_position = p.new_position;

            // compute velocity
            vector_type fgrav = gravity_ * p.mass;
            vector_type a = fgrav * p.i_density_balance * dt;
            v += a;
            real_type speed = Traits::length_sq(vdt);
            C_ =(std::max)(speed, C_);

            // advance to predicted position
            p.new_position += v *(dt * dumping_);
# else
            // hand optimized code

            // use previous position to compute next velocity
            vector_type vdt = p.new_position - p.old_position;

            // save previous position
            p.old_position = p.new_position;

            // compute velocity
            vector_type adt =
                gravity_ *(p.mass * p.i_density_balance * dt2);
            vdt += adt;
            real_type speed = Traits::length_sq(vdt);
            C_ =(std::max)(speed, C_);

            // advance to predicted position
            p.new_position += vdt * dumping_;
#endif
        }

        C_ = sqrtf(C_);

        {
            // PerformanceCounter pc(false);
            update_pairs();
            // pc.print("update_pairs");
            compute_plain_density();
            // pc.print("compute_plain_density");
            designate_boundary();
            // pc.print("designate_boundary");
            compute_density();
            // pc.print("compute_density");
            normalize_density();
            // pc.print("normalize_density");
            double_density_relaxation1(dt);
            // pc.print("double_density_relaxation1");
            double_density_relaxation2(dt);
            // pc.print("double_density_relaxation2");
        }

#if 0
        const float BOX_WIDTH = 400.0f / src_search_radius_; // 40cmの箱
        const float BOX_HEIGHT = 400.0f / src_search_radius_; // 40cmの箱

        // resolve collisions
        for (size_t i = 0 ; i <particles_.size(); i++) {
            particle_type& p = particles_[i];
            real_type minx = 0 + case_offset_.x;
            if (p.new_position.x <minx) {
                p.new_position.x = minx -(minx - p.new_position.x)* 0.80f;
            }
            real_type maxx = BOX_WIDTH + case_offset_.x;
            if (maxx <p.new_position.x) {
                p.new_position.x = maxx +(p.new_position.x - maxx)* 0.80f;
            }
            real_type maxy = BOX_HEIGHT + case_offset_.y;
            if (maxy <p.new_position.y) {
                p.new_position.y = maxy +(p.new_position.y - maxy)* 0.80f;
            }
        }
#endif
    }

    void set_case_offset(const vector_type& v) {
        case_offset_ = v / src_search_radius_;
    }

    void set_viscosity( real_type v ) { viscosity_ = v; }
    real_type get_viscosity() { return viscosity_; }
    void set_dumping(real_type d) {
        if (1.0f <d) { d = 1.0f; }
        dumping_ = d;
    }
    real_type get_dumping() { return dumping_; }

    void set_ideal_density(float x) {
        ideal_density_ = x;
        for (int i = 0 ; i <int(particles_.size()); i++) {
            particle_type& pi = particles_[i];
            pi.density0 = x;
        }
    }

    float get_ideal_density() {
        return ideal_density_;
    }
						   


private:
    void update_pairs() {
        const vector_type diag = Traits::diag_vector();

        detail::HashTable<Traits> ht(particles_);
        pairs_.clear();

        for (int i = 0 ; i <int(particles_.size()); i++) {
            particle_type& pi = particles_[i];

            int coords[2][Traits::DIMENSION];
            Traits::make_coords(coords[0], pi.new_position - diag);
            Traits::make_coords(coords[1], pi.new_position + diag);

            // traverse 
            pi.density_plain  = 0;
            pi.density_balance  = 0;
            pi.density_repulsive = 0;

            int c[Traits::DIMENSION];
            detail::update_pairs_n<Traits>::exec(
                pi, ht, coords, c, pairs_);
        }
    }
	
    void compute_plain_density() {
        real_type k = kernelc();

        for (int i = 0 ; i <int(particles_.size()); i++) {
            particle_type& pi = particles_[i];
            pi.density_plain = k;
        }

        for (int i = 0 ; i <int(pairs_.size()); i++) {
            pair_type& pair = pairs_[i];
            particle_type& pi = *pair.car;
            particle_type& pj = *pair.cdr;

            real_type kernel = k * kernel3(pair.length_sq);
            pi.density_plain += kernel;
            pj.density_plain += kernel;
        }
    }
	
    void designate_boundary() {
        real_type k = kernelc();

        for (int i = 0 ; i <int(particles_.size()); i++) {
            particle_type& pi = particles_[i];
            pi.i_density_plain = real_type(1)/ pi.density_plain;
            pi.boundariness = k * pi.i_density_plain;
        }

        for (int i = 0 ; i <int(pairs_.size()); i++) {
            pair_type& pair = pairs_[i];
            particle_type& pi = *pair.car;
            particle_type& pj = *pair.cdr;

            float kernel = k * kernel3(pair.length_sq);
            pi.boundariness += pj.mass * kernel * pj.i_density_plain;
            pj.boundariness += pi.mass * kernel * pi.i_density_plain;
        }
    }

    void compute_density() {
        for (int i = 0 ; i <int(particles_.size()); i++) {
            particle_type& pi = particles_[i];
            pi.density_balance    = pi.mass;
            pi.density_repulsive  = pi.mass;
        }

        for (int i = 0 ; i <int(pairs_.size()); i++) {
            pair_type& pair = pairs_[i];
            particle_type& pi = *pair.car;
            particle_type& pj = *pair.cdr;

            real_type k2 = kernel2(pair.length);
            real_type k3 = kernel3(pair.length);

            pi.density_balance += pj.mass * k2;
            pi.density_repulsive += pj.mass * k3;

            pj.density_balance += pi.mass * k2;
            pj.density_repulsive += pi.mass * k3;
        }
    }
	
    void normalize_density() {
        for (int i = 0 ; i <int(particles_.size()); i++) {
            particle_type& pi = particles_[i];
            pi.i_density_balance = real_type(1)/ pi.density_balance;
            pi.i_density_repulsive = real_type(1)/ pi.density_repulsive;
            pi.density_balance_numerator = pi.mass;
            pi.density_balance_denominator = pi.mass *pi.i_density_balance;
            pi.density_repulsive_numerator = pi.mass;
            pi.density_repulsive_denominator = pi.mass *pi.i_density_repulsive;
        }

        for (int i = 0 ; i <int(pairs_.size()); i++) {
            pair_type& pair = pairs_[i];
            particle_type& pi = *pair.car;
            particle_type& pj = *pair.cdr;

            real_type k2 = kernel2(pair.length);
            real_type k3 = kernel3(pair.length);

            if (real_type(1.0) <= pj.boundariness) {
                real_type b = pj.mass * k2;
                real_type r = pj.mass * k3;
                pi.density_balance_numerator   += b;
                pi.density_balance_denominator += b * pj.i_density_balance;
                pi.density_repulsive_numerator += r;
                pi.density_repulsive_denominator += r * pj.i_density_repulsive;
            }

            if (real_type(1.0) <= pi.boundariness) {
                real_type b = pi.mass * k2;
                real_type r = pi.mass * k3;
                pj.density_balance_numerator   += b;
                pj.density_balance_denominator += b * pi.i_density_balance;
                pj.density_repulsive_numerator += r;
                pj.density_repulsive_denominator += r * pi.i_density_repulsive;
            }
        }
    }
	
    void double_density_relaxation1(float dt) {
        for (int i = 0 ; i <int(particles_.size()); i++) {
            particle_type& pi = particles_[i];
            //pi.move = Traits::zero_vector();

            if (Traits::epsilon() <= pi.density_balance_denominator) {
                pi.density_balance_corrected =
                    pi.density_balance_numerator /
                    pi.density_balance_denominator;
            } else {
                pi.density_balance_corrected = pi.density_balance;
            }

            if (Traits::epsilon() <= pi.density_repulsive_denominator) {
                pi.density_repulsive_corrected =
                    pi.density_repulsive_numerator /
                    pi.density_repulsive_denominator;
            } else {
                pi.density_repulsive_corrected = pi.density_repulsive;
            }

            pi.pressure_balance =
                pressure_balance_coefficient_ *
                (pi.density_balance_corrected - pi.density0);
            pi.pressure_repulsive =
                pressure_repulsive_coefficient_ *
                pi.density_repulsive_corrected;
        }
    }

    void double_density_relaxation2(float dt) {
        real_type dt2 = square(dt);
        real_type dt2_half = dt2 * real_type(0.5);

        for (int i = 0 ; i <int(pairs_.size()); i++) {
            pair_type& pair = pairs_[i];
            particle_type& pi = *pair.car;
            particle_type& pj = *pair.cdr;

            vector_type v_n = pair.diff;
            if (Traits::epsilon()<pair.length) { v_n /= pair.length; }

            real_type lengthr = real_type(1)- pair.length;
            real_type lengthr_sq = square(lengthr);

            real_type Di_half =
                dt2_half *
                (pi.pressure_balance * lengthr +
                 pi.pressure_repulsive * lengthr_sq);
            real_type Dj_half =
                dt2_half *
                (pj.pressure_balance * lengthr +
                 pj.pressure_repulsive * lengthr_sq);

            vector_type npr = v_n * (Di_half + Dj_half);

            pj.new_position += npr;
            //pj.move += npr;
            pi.new_position -= npr;
            //pi.move -= npr;
        }
    }

private:
    inline real_type square(real_type x) { return x * x; }

private:
    std::vector<particle_type>   particles_;
    std::vector<pair_type>       pairs_;
    real_type               C_;
    real_type               src_search_radius_;
    real_type               viscosity_;
    real_type               dumping_;
    vector_type             gravity_;
    real_type               ideal_density_;
    real_type               pressure_balance_coefficient_;
    real_type               pressure_repulsive_coefficient_;
    vector_type             case_offset_;

};

} // namespace sph

#endif // SPH_HPP_
