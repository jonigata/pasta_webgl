// 2018/01/03 Naoyuki Hirayama

/*!
	@file	  sph_d3d.hpp
	@brief	  <ŠT—v>

	<à–¾>
*/

#ifndef SPH_D3D_TRAITS_HPP_
#define SPH_D3D_TRAITS_HPP_

struct SPH_Traits_D3DX_2D {
    typedef float  real_type;
    typedef D3DXVECTOR2 vector_type;
    typedef struct {} load_type;
    enum { DIMENSION = 2 };

    static real_type epsilon() {
        return 1.0e-6f;
    }
    static vector_type zero_vector() {
        return vector_type(0.0f, 0.0f);
    }
    static vector_type diag_vector() {
        return vector_type(1.0f, 1.0f);
    }
    static real_type length(const vector_type& v) {
        return D3DXVec2Length(&v);
    }
    static real_type length_sq(const vector_type& v) {
        return D3DXVec2LengthSq(&v);
    }

    static int coord(real_type n) {
        return int(floor(n));
    }
    static void make_coords(int a[2], const vector_type& v) {
        a[0] = coord(v.x);
        a[1] = coord(v.y);
    }
    static void make_vector(vector_type& v, const real_type a[2]) {
        v.x = a[0];
        v.y = a[1];
    }
    static int hash(const int a[2], int table_size) {
        // large prime numbers
        const int p1 = 73856093;
        const int p2 = 19349663;

        return size_t((a[0] * p1)^(a[1] * p2))% table_size;
    }

};

#endif // SPH_D3D_TRAITS_HPP_
