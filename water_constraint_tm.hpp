// 2018/01/09 Naoyuki Hirayama

/*!
	@file	  water_constraint_tm.hpp
	@brief	  <ŠT—v>

	<à–¾>
*/

#ifndef WATER_CONSTRAINT_TM_HPP_
#define WATER_CONSTRAINT_TM_HPP_

#include "water.hpp"
#include "segment_property.hpp"
#include "trapezoidal_map.hpp"

class WaterConstraint_TrapezoidalMap : public IConstraint {
public:
    WaterConstraint_TrapezoidalMap(
        gci::Document<Vector2>& doc,
        TrapezoidalMapMachine<float, SegmentProperty>& tmm) 
        : document_(doc), tmm_(tmm), offset_(0, 0) { }

    Vector2 apply(const Vector2& vv) {
        Vector2 v = vv;

        float minx = 1.0f + offset_.x;
        float miny = 1.0f + offset_.y;
        float maxx = 511.0f + offset_.x;
        float maxy = 511.0f + offset_.y;

        if (v.x <minx) { v.x = minx; }
        if (v.y <miny) { v.y = miny; }
        if (maxx <= v.x) { v.x = maxx; }
        if (maxy <= v.y) { v.y = maxy; }

#if 1
        const gci::Document<Vector2>& d = document_;

        Vector2 qv(v.x - offset_.x, v.y - offset_.y);
        TrapezoidalMap<float, SegmentProperty>::Point q(qv.x, qv.y);

        int score;
        SegmentProperty tsp;
        SegmentProperty bsp;

        
        if (tmm_.find(q, score, tsp, bsp)) {
            if (0 <= tsp.lower_cell_index) {
                const auto& cell = d.voronoi_cells[tsp.lower_cell_index];
                const auto& site = d.sites[cell.site_index];

                Vector2 qv2;
                if (site.is_segment) {
                    qv2 = nearest_point_on_line(
                        d.vertices[site.p0],
                        d.vertices[site.p1],
                        qv);
                } else {
                    qv2 = document_.vertices[site.p0];
                }

                v = (qv2 - qv) * 0.5f + qv;
                v += offset_;
            }
        }
#endif

        return v;
    }

    void set_offset(const Vector2& v) { offset_ = v; }

private:
    Vector2 nearest_point_on_line(
        const Vector2& p0,
        const Vector2& p1,
        const Vector2& q) {
        float dx = p1.x - p0.x;
        float dy = p1.y - p0.y;
        float a = dx * dx + dy * dy;
        if (a == 0) { return p0; }

        float b = dx * (p0.x - q.x) + dy * (p0.y - q.y);
        float t = -(b / a);
        if (t < 0.0f) { t = 0.0f; }
        if (1.0f < t) { t = 1.0f; }
        return Vector2(p0.x + dx * t, p0.y + dy * t);
    }
        
private:
#if 1
    gci::Document<Vector2>                          document_;
    TrapezoidalMapMachine<float, SegmentProperty>&  tmm_;
#endif
    Vector2         offset_;
};


#endif // WATER_CONSTRAINT_TM_HPP_
