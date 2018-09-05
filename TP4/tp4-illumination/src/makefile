ifneq ($(VGL_CLIENT),)
    VGLRUN= vglrun
endif

CONTEXT=sdl2
ifeq "$(shell uname)" "Darwin"
    CONTEXT=glfw3
    LDFLAGS += -lobjc -framework Foundation -framework OpenGL -framework Cocoa
endif

CXXFLAGS += -g -W -Wall -Wno-unused-parameter -Wno-deprecated-declarations
CXXFLAGS += $(shell pkg-config --cflags glew)
CXXFLAGS += $(shell pkg-config --cflags $(CONTEXT))

LDFLAGS += -g
LDFLAGS += $(shell pkg-config --libs glew)
LDFLAGS += $(shell pkg-config --libs $(CONTEXT))

SRC=main

all : $(SRC).exe
run : $(SRC).exe
	$(VGLRUN) ./$(SRC).exe
$(SRC).exe : $(SRC).cpp *.h
	$(CXX) $(CXXFLAGS) -o$@ $(SRC).cpp $(LDFLAGS)

sol :  ; make SRC=$(SRC)Solution all
runs : ; make SRC=$(SRC)Solution run

clean :
	rm -rf *.o *.exe *.exe.dSYM
