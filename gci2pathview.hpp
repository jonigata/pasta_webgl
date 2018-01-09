// 2018/01/01 Naoyuki Hirayama

/*!
	@file	  gci2pathview.hpp
	@brief	  <ŠT—v>

	<à–¾>
*/

#ifndef GCI2PATHVIEW_HPP_
#define GCI2PATHVIEW_HPP_

#include "gci.hpp"
#include "pathview.hpp"
#include "region_color.hpp"
#include "trapezoidal_map.hpp"
#include <map>

namespace gci2pathview {

struct SegmentProperty {
    int upper_cell_index;
    int lower_cell_index;
};

inline
void post_random_color(std::vector<pathview::Primitive>& pb, int& color_index) {
    unsigned long color;
    do {
        color = get_region_color(color_index++);
    } while (get_color_distance(0xffffff, color) < 0.1f);

    float r =((color & 0xff0000)>> 16)/ 255.0f;
    float g =((color & 0x00ff00)>>  8)/ 255.0f;
    float b =((color & 0x0000ff))/ 255.0f;

    pathview::Primitive p;
    p.opcode = pathview::Primitive::Color;
    p.operands[0] = r;
    p.operands[1] = g;
    p.operands[2] = b;
    pb.push_back(p);
}

template <class Vec>
void draw_document(
    const gci::Document<Vec>& doc,
    std::vector<pathview::Primitive>& primitives) {

    // LINE
    pathview::Primitive p;
    p.opcode = pathview::Primitive::Color;
    p.operands[0] = 0;
    p.operands[1] = 0;
    p.operands[2] = 0;
    primitives.push_back(p);

    for (size_t i = 0 ; i < doc.voronoi_cells.size() ; i++) {
        const std::vector<int>& v = doc.voronoi_cells[i].vertex_indices;
        pathview::Primitive p;
        p.opcode = pathview::Primitive::MoveTo;
        p.operands[0] = doc.vertices[v.back()].x;
        p.operands[1] = doc.vertices[v.back()].y;
        primitives.push_back(p);
        for (size_t j = 0 ; j < v.size(); j++) {
            p.opcode = pathview::Primitive::LineTo;
            p.operands[0] = doc.vertices[v[j]].x;
            p.operands[1] = doc.vertices[v[j]].y;
            primitives.push_back(p);
        }
    }

    // POLYGON
    int color_index = 0;
    for (size_t i = 0 ; i < doc.voronoi_cells.size(); i++) {
        post_random_color(primitives, color_index);

        const std::vector<typename gci::Document<Vec>::Triangle>& v =
            doc.voronoi_cells[i].triangles;

        for (size_t j = 0 ; j < v.size(); j++) {
            p.opcode = pathview::Primitive::MoveTo;
            p.operands[0] = doc.vertices[v[j].v0].x;
            p.operands[1] = doc.vertices[v[j].v0].y;
            primitives.push_back(p);

            p.opcode = pathview::Primitive::Triangle;
            p.operands[0] = doc.vertices[v[j].v1].x;
            p.operands[1] = doc.vertices[v[j].v1].y;
            p.operands[2] = doc.vertices[v[j].v2].x;
            p.operands[3] = doc.vertices[v[j].v2].y;
            primitives.push_back(p);
        }
    }

    printf("primitives: %d\n", primitives.size());
}

template <class Vec, class SegmentProperty>
void gci_to_trapezoidal_map(
    const gci::Document<Vec>& doc,
    TrapezoidalMap<float, SegmentProperty>& tm) {
    
    // point location
    std::vector<typename TrapezoidalMap<float, SegmentProperty>::Point> points;
    for (size_t i = 0 ; i < doc.vertices.size() ; i++) {
        points.push_back(
            TrapezoidalMap<float, SegmentProperty>::make_point(
                doc.vertices[i].x,
                doc.vertices[i].y));
    }

    int m = 0;
    std::map<std::pair<int, int>, SegmentProperty> segments;
    for (size_t i = 0 ; i < doc.voronoi_cells.size(); i++) {
        const std::vector<int>& v = doc.voronoi_cells[i].vertex_indices;

        for (size_t j = 0 ; j < v.size(); j++) {
            int index0 = v[j];
            int index1 = v[(j+1)% v.size()];

            // lexicographical compare
            bool invert = !(points[index0] < points[index1]);

            if (index1 < index0) { std::swap(index0, index1); }
            std::pair<int, int> s = std::make_pair(index0, index1);

            if (segments.find(s) == segments.end()) {
                SegmentProperty sp;
                sp.upper_cell_index = -1;
                sp.lower_cell_index = -1;
                if (invert) {
                    sp.upper_cell_index = int(i);
                } else {
                    sp.lower_cell_index = int(i);
                }
                segments[s] = sp;
                m++;
                //tm_.add_segment(points[index0], points[index1]);
            } else {
                SegmentProperty& sp = segments[s];
                if (invert) {
                    sp.upper_cell_index = int(i);
                } else {
                    sp.lower_cell_index = int(i);
                }
            }
        }
    }

    for (const auto& p: segments) {
#if 0
        dprintf_real(
            "segment adjacent faces: %d, %d\n",
            p.second.upper_cell_index,
            p.second.lower_cell_index);
#endif

        tm.add_segment(
            points[p.first.first],
            points[p.first.second],
            true,
            p.second);
    }
}

}

#endif // GCI2PATHVIEW_HPP_
