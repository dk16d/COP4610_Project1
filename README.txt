Riley Corey
Darren Kopacz
Valion Joyce

COP4610 - Project 1

******************************************************************************

~~~ SUMMARY ~~~
File listing:
proj1.c
makefile
README.text

Makefile description: 
run make, type ./shell


shell: proj1.o
	gcc -std=c99 -o shell proj1.o

proj1.o: proj1.c
	gcc -std=c99 -c proj1.c

clean:
	rm *.o shell




~~~ KNOWN ISSUES ~~~
Part 9 - Background Processing: Processes "stack" on top of one another; Once they have finished they do not exit the array. 
                                Does not support piping or redirection.
Part 10 - Jobs & cd not implemented 
          exit does not wait for any background processes that are still running
          does not prints the number of commands (external and internal) executed


~~~ CONTRIBUTIONS ~~~
Darren Kopacz:
* Created GitHub repository and assisted team with using git on linprog.
* Added variable dectection for PART 2. [ Ex) dectects '$' in $USER ].
* Optimizations for PART 2, removed redundant variable initalization.
* Added function prototypes and codeblock structure for PART 5.
* Included error checking for environment variables.
* Reworked Part 1 through Part 4 to save all expansions in token list.

Valion Joyce:
* Did the implemenation and optimization of code and for parts 1,2,3,4,5,7,8.
* optomized the makefile 

Riley Corey:
* Implementations for Part 6,9
* (Part 10) Built in - echo
* Documented instructions and pseudocode for path search & background processing.
* Attempted optimization for background processing with Darren.
* Wrote original makefile

~~~ GIT COMMIT LOG ~~~

