// 2013/04/27 Naoyuki Hirayama

#include <cstdio>
#include "screen.hpp"
#include "jslib.hpp"
//#include "texture.hpp"
#include "gl.hpp"

void start(int argc, const char *argv[]) {
    initMouse();
    initTextureVault();

    Screen screen(argc, argv, "yamadumi");

    screen.on_keyboard(
        [&](int code) {
            printf("%d\n", code);
            switch (code) {
                case ' ':
                    // bindings->add_body(screen);
                    break;
            }
        });
    
    screen.on_idle(
        [=](float){
            // bindings->update();
        });

    screen.do_main_loop();
}


int main(int argc, const char *argv[]) {
    start(argc, argv);
    return 0;
}
