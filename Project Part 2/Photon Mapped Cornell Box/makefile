CC = g++
LIBS = -fopenmp -lGL -lGLU -lglfw -lGLEW
CFLAGS = -O3
DEPS = constants.cpp utils.cpp shapes.cpp scene.cpp main.cpp


cornell_box: $(DEPS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -f *.o *~ cornell_box
