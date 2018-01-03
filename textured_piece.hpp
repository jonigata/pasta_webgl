// 2014/09/20 Naoyuki Hirayama

#ifndef TEXTURED_PIECE_HPP_
#define TEXTURED_PIECE_HPP_

#include "geometry.hpp"
#include "color.hpp"
#include "gl.hpp"
#include "texture.hpp"
#include <vector>
#include <string>

class TexturedPiece {
public:
    TexturedPiece() {
        vbo_ = 0;
        ibo_ = 0;
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
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, p + 3);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride, p + 6);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, p + 10);

        /* Enable the attributes */
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);

        bindTexture(texture_.c_str());

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
        glDisableVertexAttribArray(3);
        glDisableVertexAttribArray(2);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void set_texture(const std::string& s) {
        texture_ = s;
    }

    void add_vertex(
        float x, float y, float z,
        float nx, float ny, float nz,
        Color c,
        float u, float v){
        Vertex dst;
        dst.position = Vector(x, y, z);
        dst.normal = Vector(nx, ny, nz);
        dst.diffuse = c;
        dst.u = u;
        dst.v = v;
        vertex_source_.push_back(dst);
    }

    void add_index(int n) {
        index_source_.push_back(n);
    }
    
    void build() {
        loadTexture(texture_.c_str());

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

private:    
    struct Vertex {
        Vector      position;
        Vector      normal;
        Color       diffuse;
        float       u;
        float       v;
    };

    typedef uint16_t Index;

    std::vector<Vertex>     vertex_source_;
    std::vector<Index>      index_source_;
    GLuint                  vbo_;
    GLuint                  ibo_;
    std::string             texture_;
};

#endif // TEXTURED_PIECE_HPP_
