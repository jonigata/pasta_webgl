// 2018/01/01 Naoyuki Hirayama

/*!
	@file	  gci.hpp
	@brief	  <概要>

	<説明>
*/

#ifndef GCI_HPP_
#define GCI_HPP_

#include <vector>
#include <cassert>

namespace gci {

template <class Vec>
struct Document {
    struct Site {
        bool    is_segment;
        int     p0;
        int     p1;
    };
    struct Triangle {
        int	v0;
        int	v1;
        int	v2;
    };
    struct Cell {
        int                     site_index;
        std::vector<int>        vertex_indices;
        std::vector<Triangle>   triangles;
    };

    std::vector<Vec>                vertices;
    std::vector<Site>               sites;
    std::vector<std::vector<int>>   input_polygons;
    std::vector<Cell>               voronoi_cells;
};

template <class IS, class Vec>
void read_gci(IS& ifs, Document<Vec>& doc) {
    // 頂点リスト
    int vertex_count;
    ifs >> vertex_count;
    for (int i = 0 ; i < vertex_count ; i++) {
        int id;
        float x, y;
        ifs >> id >> x >> y;
        doc.vertices.push_back(Vec(x, y));
    }
    printf("vertex_count: %d\n", vertex_count);

    // 入力ポリゴン
    int input_polygon_count;
    ifs>> input_polygon_count;
    for (int i = 0 ; i < input_polygon_count ; i++) {
        int id, c;
        ifs >> id >> c;
        std::vector<int> p;
        for (int j = 0 ; j < c ; j++) {
            int index;
            ifs >> index;
            p.push_back(index);
        }
        doc.input_polygons.push_back(p);
    }
    printf("input_polygon_count: %d\n", input_polygon_count);

    // 入力サイト
    int input_site_count;
    ifs >> input_site_count;
    for (int i = 0 ; i < input_site_count ; i++) {
        int id, type;
        ifs >> id >> type;
        typename Document<Vec>::Site s;
        if (type == 1) {
            s.is_segment = false;
            ifs >> s.p0;
        } else {
            assert(type == 2);
            s.is_segment = true;
            ifs >> s.p0 >> s.p1;
        }
        doc.sites.push_back(s);
    }
    printf("input_site_count: %d\n", input_site_count);

    // ボロノイセル・三角形分割
    int voronoi_cell_count;
    ifs >> voronoi_cell_count;
    for (int i = 0 ; i < voronoi_cell_count ; i++) {
        typename Document<Vec>::Cell cell;
        int id;
        ifs >> id >> cell.site_index;

        int c;
        ifs >> c;
        for (int j = 0 ; j < c ; j++) {
            int index;
            ifs >> index;
            cell.vertex_indices.push_back(index);
        }

        ifs >> c;
        for (int j = 0 ; j <c ; j++) {
            typename Document<Vec>::Triangle t;
            ifs >> t.v0 >> t.v1 >> t.v2;
            cell.triangles.push_back(t);
        }

        doc.voronoi_cells.push_back(cell);
    }
    printf("voronoi_cell_count: %d\n", voronoi_cell_count);
}

}



#endif // GCI_HPP_
