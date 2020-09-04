shell: proj1.o
	gcc -std=c99 -o shell proj1.o

proj1.o: proj1.c
	gcc -std=c99 -c proj1.c

clean:
	rm *.o shell
