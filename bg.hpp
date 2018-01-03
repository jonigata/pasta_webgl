// 2014/09/20 Naoyuki Hirayama

#ifndef BG_HPP_
#define BG_HPP_

#include "textured_piece.hpp"

class BG {
public:
    BG() {
        piece_.set_texture("city_night002.png");
        add_vertex(0, 0);
        add_vertex(1, 0);
        add_vertex(0, 1);
        add_vertex(1, 1);
        piece_.add_index(0);
        piece_.add_index(1);
        piece_.add_index(2);
        piece_.add_index(2);
        piece_.add_index(1);
        piece_.add_index(3);

        piece_.build();
    }

    void add_vertex(float x, float y){
        Color c {{1.0f ,1.0f ,1.0f, 1.0f}};
        piece_.add_vertex(x, y, 0, 0, 0, 1, c, x, 1-y);
    }

    void render() {
        piece_.render();
    }
        

private:
    TexturedPiece piece_;
};

#endif // BG_HPP_
