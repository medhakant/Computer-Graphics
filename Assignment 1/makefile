CC = g++
LIBS = -lGL -lGLU -lglfw -lGLEW -fopenmp
DEPS = 
OBJ = main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

snowman: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -f *.o *~ snowman
