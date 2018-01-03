// 2009/01/29 Naoyuki Hirayama

/*!
	@file	  renderer.hpp
	@brief	  <ŠT—v>

	<à–¾>
*/

#ifndef PATHVIEW_RENDERER_GL_HPP_
#define PATHVIEW_RENDERER_GL_HPP_

#include "vector2d.hpp"
#include "geometry.hpp"
#include "vector4d.hpp"
#include "pathview.hpp"
#include "color.hpp"
#include "gl.hpp"
#include "piece.hpp"
#include <vector>

class PathviewRenderer {
private:
    struct Vertex {
        Vector  pos;
        Color   color;

        void operator()(float xx,float yy,float zz,const Color& cc) {
            pos.x = xx; pos.y = yy; pos.z = zz; color = cc;
        }
    };


public:
    PathviewRenderer() {
        // offset_ = Vector2(-128, -128);
        zoom_ = 1 * ZOOM_DIVISION;
        init();
    }
    ~PathviewRenderer() {}

    void build(
        int width,
        int height,
        const std::vector<pathview::Primitive>& primitives,
        int step);
    void render();

private:
    void init();

    void draw_primitives(
        const std::vector<pathview::Primitive>& primitives,
        int                                     step,
        Vertex*&                                v0,
        Vertex*&                                v1);

    Vector2 vec2(const float* a) {
        return Vector2(a[0], a[1]);
    }

    enum {
        ZOOM_DIVISION = 1000,
    };

    Vector mag(int width, int height, const Vector2& v) {
        return Vector(v.x / width, v.y / height, 0);
    }

private:
    Vector2    offset_;
    Vector2    tmp_offset_;
    int      zoom_;

    Piece piece_;
};

#endif // PATHVIEW_RENDERER_GL_HPP_
