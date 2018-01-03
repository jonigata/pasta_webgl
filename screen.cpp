// 2013/04/27 Naoyuki Hirayama

#include "screen.hpp"

#include <cstdio>
#include <cmath>
#include <cassert>
#include <vector>
#include <fstream>

#include "gl.hpp"
#include "bg.hpp"
#include "gci.hpp"
#include "vector2d.hpp"
#include "pathview.hpp"
#include "gci2pathview.hpp"
#include "pathview_renderer_gl.hpp"
#include "mouse_dispatcher.hpp"
#include "camera.hpp"
#include "geometry.hpp"
#include "shader.hpp"
#include "water_in_bottle.hpp"

static ScreenImp* imp_ = nullptr;

void sincos(float angle, float* s, float* c) {
    *s = sin(angle);
    *c = cos(angle);
}

class ScreenImp {
public:
    ////////////////////////////////////////////////////////////////
    // public interface
    ScreenImp(int argc, const char** argv, const char* title)
        : LightSourcePosition_ {5.0, 5.0, 10.0, 1.0} {

        init(argc, argv, title);
        bg_ = std::make_shared<BG>();
        pathview_ = std::make_shared<PathviewRenderer>();

        gci::Document<Vector2> doc;
        std::ifstream ifs("data/cave.gci");
        read_gci(ifs, doc);
        gci2pathview::draw_document(doc, pathview_primitives_);
        pathview_->build(512, 512, pathview_primitives_, 0);

        mouse_dispatcher_.add_acceptor(&camera_, 0);
    }

    ~ScreenImp() {
        mouse_dispatcher_.remove_acceptor(&camera_);
    }

    void mouse(int which, int kind, int x, int y) {
        bool* b = nullptr;
        switch(which) {
            case 1: b = &mouse_state_.lbutton; break;
            case 2: b = &mouse_state_.mbutton; break;
            case 3: b = &mouse_state_.rbutton; break;
        }

        switch(kind) {
            case 0:
                if (b) {
                    *b = true;
                }
                break;
            case 2:
                if (b) {
                    *b = false;
                }
                break;
        }
        mouse_state_.position = Point(x, y);
        mouse_dispatcher_.on_mouse_message(mouse_state_);
    }

    void on_keyboard(std::function<void (int)> f) {
        on_keyboard_ = f;
    }

    void on_special(std::function<void (int,int,int)> f) {
        on_special_ = f;
    }

    void on_idle(std::function<void (float)> f) {
        printf("setting on_idle\n");
        on_idle_ = f;
    }

    void do_main_loop() {
        glutMainLoop();
    }

    ////////////////////////////////////////////////////////////////
    // glut callback
    void idle() {
        static int frames = 0;
        static double tRot0 = -1.0, tRate0 = -1.0;
        double dt, t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;

        if (tRot0 < 0.0)
            tRot0 = t;
        dt = t - tRot0;
        tRot0 = t;

        /* advance rotation for next frame */
        if (on_idle_) {
            on_idle_(dt);
        }

        glutPostRedisplay();
        frames++;

        if (tRate0 < 0.0)
            tRate0 = t;
        if (t - tRate0 >= 5.0) {
            GLfloat seconds = t - tRate0;
            GLfloat fps = frames / seconds;
            printf("%d frames in %3.1f seconds = %6.3f FPS\n", frames, seconds,
                   fps);
            tRate0 = t;
            frames = 0;
        }

        water_.update();
    }

    void reshape(int width, int height) {
        /* Update the projection matrix */
        //perspective(ProjectionMatrix_, 60.0, width / (float)height, 1.0, 1024.0);
        ProjectionMatrix_ =
            perspective_fov_rh(
                DEG2RAD(60.0), width / (float)height, 1.0, 1024.0);

        /* Set the viewport */
        glViewport(0, 0, (GLint) width, (GLint) height);
    }

    void draw() {
        // glClearColor(1.0, 1.0, 1.0, 0.0);
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // draw_bg();
        draw_pathview();
        water_.render();

        glutSwapBuffers();
    }

    void keyboard(int code) {
        on_keyboard_(code);
    }

    void special(int special, int crap, int morecrap) {
        on_special_(special, crap, morecrap);
    }

    Vector make_view_point() {
        return camera_.make_view_point();
    }

private:
    void init(int argc, const char** argv, const char* title) {
        glutInit(&argc, const_cast<char**>(argv));
        glutInitWindowSize(640, 640);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

        glutCreateWindow(title);

        /* Set up glut callback functions */
        glutIdleFunc ([](){ imp_->idle(); });
        glutReshapeFunc([](int w, int h) { imp_->reshape(w, h); });
        glutDisplayFunc([](){ imp_->draw(); });
        glutKeyboardFunc([](unsigned char c, int, int){ imp_->keyboard(c); });
        glutSpecialFunc([](int s, int c, int m){ imp_->special(s, c, m); });

        room_shader_.reset(new RoomShader);
        figure_shader_.reset(new FigureShader);
    }

    void draw_bg() {
        figure_shader_->attach(LightSourcePosition_);

        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);

        Matrix bg_matrix =
            Matrix::scale(2.0, 2.0, 1.0) * 
            Matrix::translate(-0.5, -0.5, 0);

        Color c {{1.0, 1.0, 1.0, 1.0}};
        
        figure_shader_->bind(
            c,
            Matrix::identity(),
            Matrix::identity(),
            bg_matrix);
        bg_->render();
    }

    void draw_pathview() {
        Matrix bg_matrix =
            Matrix::scale(2.0, 2.0, 1.0) * 
            Matrix::translate(-0.5, -0.5, 0);

        room_shader_->attach();
        room_shader_->bind(
            Matrix::identity(),
            Matrix::identity(),
            bg_matrix);

        pathview_->render();
    }

private:
    MouseDispatcher mouse_dispatcher_;
    Camera          camera_;
    MouseState      mouse_state_;

    Matrix ProjectionMatrix_;
    const GLfloat LightSourcePosition_[4];

    std::unique_ptr<RoomShader> room_shader_;
    std::unique_ptr<FigureShader> figure_shader_;

    std::shared_ptr<BG> bg_;
    std::shared_ptr<PathviewRenderer> pathview_;
    std::function<void (int)> on_keyboard_;
    std::function<void (int, int, int)> on_special_;
    std::function<void (float)> on_idle_;

    std::vector<pathview::Primitive> pathview_primitives_;
    
    WaterInBottle water_;

};

/*============================================================================
 *
 * class Screen 
 *
 * 
 *
 *==========================================================================*/
//<<<<<<<<<< Screen

//****************************************************************
// constructor
Screen::Screen(int argc, const char** argv, const char* title){
    assert(imp_ == nullptr);
    printf("Screen::Screen1\n");
    imp_ = new ScreenImp(argc, argv, title);
    printf("Screen::Screen2\n");
}

//****************************************************************
// destructor
Screen::~Screen() {
    delete imp_;
}

//****************************************************************
// do_main_loop
void Screen::do_main_loop() {
    printf("Screen::do_main_loop\n");
    imp_->do_main_loop();
}

//****************************************************************
// on_keyboard
void Screen::on_keyboard(std::function<void (int)> f) {
    imp_->on_keyboard(f);
}

//****************************************************************
// on_special
void Screen::on_special(std::function<void (int,int,int)> f) {
    imp_->on_special(f);
}

//****************************************************************
// on_idle
void Screen::on_idle(std::function<void (float)> f) {
    printf("Screen::on_idle\n");
    imp_->on_idle(f);
}

//****************************************************************
// make_view_point
Vector Screen::make_view_point() {
    return imp_->make_view_point();
}

//>>>>>>>>>> Screen

extern "C" {
void addMouseEvent(int which, int kind, int x, int y) {
    //printf("%d, %d: %d, %d\n", which, kind, x, y);
    imp_->mouse(which, kind, x, y);
}

}

