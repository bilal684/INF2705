SRC=main

CXXFLAGS += -g -W -Wall -Wno-unused-parameter -Wno-deprecated-declarations
CXXFLAGS += $(shell pkg-config --cflags glew)
CXXFLAGS += $(shell pkg-config --cflags sdl2)
#CXXFLAGS += $(shell pkg-config --cflags glfw3)

LDFLAGS += -g
LDFLAGS += $(shell pkg-config --libs glew)
LDFLAGS += $(shell pkg-config --libs sdl2)
#LDFLAGS += $(shell pkg-config --libs glfw3)

all : $(SRC).exe
run : $(SRC).exe
	./$(SRC).exe
$(SRC).exe : $(SRC).cpp *.h
	$(CXX) $(CXXFLAGS) -o$@ $(SRC).cpp $(LDFLAGS)

sol : $(SRC)Solution.exe
runs : $(SRC)Solution.exe
	./$(SRC)Solution.exe
$(SRC)Solution.exe : $(SRC)Solution.cpp *.h
	$(CXX) $(CXXFLAGS) -o$@ $(SRC)Solution.cpp $(LDFLAGS)

clean :
	rm -rf *.o *.exe *.exe.dSYM
