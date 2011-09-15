Walk: main.o glm.o
	cc -o World -lglut -lgd  main.o glm.o

clean:
	rm -f *.o World
