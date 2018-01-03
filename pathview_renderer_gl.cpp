// 2009/01/29 Naoyuki Hirayama

#include "pathview_renderer_gl.hpp"
#include <cstdio>

/*============================================================================
 *
 * class PathviewRenderer
 *
 * 
 *
 *==========================================================================*/
//<<<<<<<<<< PathviewRenderer
void PathviewRenderer::init() {
}

//****************************************************************
// build
void PathviewRenderer::build(
    int w,
    int h,
    const std::vector<pathview::Primitive>& primitives,
    int                                     step) {

    Vector2 cursor(0, 0);
    Color color {{0.0f, 0.0f, 0.0f, 1.0f}};

    for (const pathview::Primitive& p: primitives) {
        const float* a = p.operands;

        switch (p.opcode) {
            case pathview::Primitive::Color:
                color = Color {{ a[0], a[1], a[2], 0.25f }};
                break;
            case pathview::Primitive::MoveTo:
                cursor = vec2(a);
                break;
            case pathview::Primitive::LineTo:
/*
                v0[0].p = vec4(offmag(cursor));
                v0[0].c = color;
                v0[1].p = vec4(offmag(vec2(a)));
                v0[1].c = color;
                v0 += 2;
*/
                cursor = vec2(a);
                break;
            case pathview::Primitive::Triangle:
                piece_.add_vi(mag(w, h, cursor), color);
                piece_.add_vi(mag(w, h, vec2(a + 0)), color);
                piece_.add_vi(mag(w, h, vec2(a + 2)), color);
                break;
            case pathview::Primitive::Dot: {
#if 0
                float r = a[0];
                D3DXVECTOR2 c = cursor;
                for (int i = 0 ; i <DOT_DIVISION ; i++) {
                    float t0 =(1.0f / DOT_DIVISION)*(i);
                    float t1 =(1.0f / DOT_DIVISION)*(i + 1);

                    t0 *= D3DX_PI * 2.0f;
                    t1 *= D3DX_PI * 2.0f;

                    float x0 = cosf(t0)* r;
                    float y0 = sinf(t0)* r;
                    float x1 = cosf(t1)* r;
                    float y1 = sinf(t1)* r;

                    v0[0].p = vec4(mag(w, h, c + D3DXVECTOR2(x0, y0)));
                    v0[0].c = color;
                    v0[1].p = vec4(mag(w, h, c + D3DXVECTOR2(x1, y1)));
                    v0[1].c = color;
                    v0 += 2;
                }
#endif
                break;
            }
            default:
                break;
        }
    }
    
    piece_.build();

}

//****************************************************************
// render
void PathviewRenderer::render() {
    piece_.render();
}

//>>>>>>>>>> PathviewRenderer

