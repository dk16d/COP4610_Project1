proj1.x: proj1.o
	gcc -std=c99 -o proj1.x proj1.o

proj1.o: proj1.c
	gcc -std=c99 -c proj1.c

clean:
	rm *.o proj1.x
