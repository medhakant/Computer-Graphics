CC = g++
LIBS = -fopenmp -lGL -lGLU -lglfw -lGLEW
CFLAGS = -O3
DEPS = scene.cpp main.cpp


cornell_box: $(DEPS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -f *.o *~ cornell_box
