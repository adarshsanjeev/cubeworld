all: cubeworld

cubeworld: cube.cpp glad.c
	clang++ -o cubeworld cube.cpp glad.c -lGLEW -lGL -ldl -lglfw

clean:
	rm cubeworld
