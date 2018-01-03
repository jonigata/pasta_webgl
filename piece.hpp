// 2014/09/20 Naoyuki Hirayama

#ifndef PIECE_HPP_
#define PIECE_HPP_

#include "geometry.hpp"
#include "color.hpp"
#include "gl.hpp"
#include <vector>
#include <string>

class Piece {
public:
    Piece() {
        vbo_ = 0;
        ibo_ = 0;
    }
    ~Piece() {
        if (vbo_ != 0) { glDeleteBuffers(1, &vbo_); }
        if (ibo_ != 0) { glDeleteBuffers(1, &ibo_); }
    }
    
    void render() {
        if (vbo_ ==0 || ibo_ == 0) {
            //printf("no contents\n");
            return ;
        }

        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);

        /* Set up the position of the attributes in the vertex buffer object */
        int stride = sizeof(Vertex);
        const float* p = 0;
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, p + 0);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, p + 3);

        /* Enable the attributes */
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

        glDrawElements(
            GL_TRIANGLES,
            index_source_.size(),
            GL_UNSIGNED_SHORT,
            nullptr);

        int error = glGetError();
        if (error != 0) {
            printf("glGetError: %d\n", error);
        }

        /* Disable the attributes */
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void add_vertex(
        float x, float y, float z,
        Color c){
        Vertex dst;
        dst.position = Vector(x, y, z);
        dst.diffuse = c;
        vertex_source_.push_back(dst);
    }

    void add_vertex(const Vector& v, Color c) {
        Vertex dst;
        dst.position = v;
        dst.diffuse = c;
        vertex_source_.push_back(dst);
    }

    void add_vertex(const Vector2& v, Color c) {
        Vertex dst;
        dst.position = Vector(v.x, v.y, 0);
        dst.diffuse = c;
        vertex_source_.push_back(dst);
    }

    void add_index(int n) {
        index_source_.push_back(n);
    }

    void add_vi(
        float x, float y, float z,
        Color c){
        add_index(vertex_source_.size());
        add_vertex(x, y, z, c);
    }

    void add_vi(const Vector& v, Color c) {
        add_index(vertex_source_.size());
        add_vertex(v, c);
    }

    void add_vi(const Vector2& v, Color c) {
        add_index(vertex_source_.size());
        add_vertex(v, c);
    }
    
    void build() {
        glGenBuffers(1, &vbo_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER,
                     vertex_source_.size() * sizeof(Vertex),
                     &vertex_source_[0],
                     GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &ibo_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     index_source_.size() * sizeof(Index),
                     &index_source_[0],
                     GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    size_t get_vertex_count() { return vertex_source_.size(); }
    size_t get_index_count() { return index_source_.size(); }

private:    
    struct Vertex {
        Vector      position;
        Color       diffuse;
    };

    typedef uint16_t Index;

    std::vector<Vertex>     vertex_source_;
    std::vector<Index>      index_source_;
    GLuint                  vbo_;
    GLuint                  ibo_;
    std::string             texture_;
};

#endif // PIECE_HPP_
