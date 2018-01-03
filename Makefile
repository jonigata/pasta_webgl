.SUFFIXES : .cpp .o .js

OBJS = pasta.o screen.o camera.o mouse_dispatcher.o region_color.o pathview_renderer_gl.o water.o
DEPS = $(OBJS:.o=.d)
JSS = pasta_lib.js 

pasta.js: $(OBJS) $(JSS)
	emcc -std=c++11 --js-library $(JSS) $(OBJS) -o pasta.js --embed-file data -s EXPORTED_FUNCTIONS="['_main','_malloc','_addMouseEvent']" -s ASSERTIONS=2 -s DEMANGLE_SUPPORT=1

.cpp.o:
	emcc -O3 -MMD -std=c++11 -Wall -Werror $< -o $@

clean:
	rm -f $(OBJS) pasta.js

dep:
	emcc -std=c++11 -MMD -MF Makefile.d -c $(OBJS:.o=.cpp)

-include $(DEPS)
