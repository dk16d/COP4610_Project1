#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>

typedef struct {
        int size;
        char **items;
} tokenlist;

char *get_input(void);
tokenlist *get_tokens(char *input);

tokenlist *new_tokenlist(void);
void add_token(tokenlist *tokens, char *item);
void free_tokens(tokenlist *tokens);
void PrintInvalid();


bool VarIsValid(char * item);
bool IsTilde(char *item);
bool CommandValid(char *commando);

//new test functions
bool IsVar(char *item);
bool IsPath(char *str);
bool FileInDir(char *str);

int main()
{
	bool execute = true;					   //Flag is set false on invalid command.
	while (1) {								   
		printf("%s@%s:%s> ", getenv("USER"), getenv("HOST"), getenv("PWD"));
											   /*PART 3 - Prompts user w/ working directory*/
		char *input = get_input();
		tokenlist *tokens = get_tokens(input);
		//printf("whole input: %s\n", input);

		for (int i = 0; i < tokens->size; i++) //Begin COMMAND EXPANSION loop.
		{
			char commando[50];			   	   //Declare modded-token buffer.
				
			if(IsVar(tokens->items[i]))	   	   //If token is an env variable...
			{								   //Strip '$' from variable.
				for(int v = 0; v < strlen(tokens->items[i]); v++)
				{							   //Copy it into commando.
					commando[v] = (tokens->items[i])[v+1];
				}
				
				if(getenv(commando) != NULL)   //IF variable exists...
				{							   //Allocate space for var expansion.
					tokens->items[i] = (char *) realloc(tokens->items[i],
						strlen(getenv(commando)) + 1);
					strcpy(tokens->items[i], getenv(commando));
				}							   //Copy expanded string to token list.
				else						   //ELSE print error, do not execute.
				{
					strcpy(tokens->items[i], commando);
					printf("%s: Undefined variable.\n");
					execute = false;
				}	
			}
											   /*PART 4 - '~' expansion*/
			if(IsTilde(tokens->items[i]))
			{								   //Strip the '~'.
				char commandsec[50];		   //(sec prevents modding of the
											   // underlying env variable.)
				for(int v = 0; v < strlen(tokens->items[i]); v++)
				{							   //Copy the rest into commando.
					commando[v] = (tokens->items[i])[v+1];
				}							   //Copy HOME path into sec.
				strcpy(commandsec, getenv("HOME"));
											   //Realloc space for expansion.	
				tokens->items[i] = (char *) realloc(tokens->items[i],
					strlen(commandsec) + strlen(commando) + 1);
				strcpy(tokens->items[i], strcat(commandsec, commando));		  
			}								   //Concat path onto expansion
											   // and copy to token list.
			printf("ToKeN %d: (%s)\n", i, tokens->items[i]);
		}


				if (tokens->size == 1)				//only commandline argument is command
				{
					char command[50];
					char commandpath[100];
					char attach[3] = "/"; 
					strcpy(commandpath, getenv("PATH"));
  					strcpy(command, tokens->items[0]);	//stores whatever command from commandline to command
					strcat(attach, command);		//attach = "/" + command example: ls is now /ls
					

					//parse path by delimiters
                                        char delim[] = ":";
                                        char *ptr = strtok(commandpath, delim);
                                        printf("%s\n", ptr);

                                        int i;
                                        i = 0;
                                        int size;
                                        size = 0;
                                        char *array[50];


                                        //printing separated directories
                                        while(ptr != NULL)                      //putting parsed directories into array
                                        {
                                                array[i++] = ptr;
                                                printf("'%s'\n", ptr);
                                                ptr = strtok(NULL, delim);
                                                size++;
                                        }

                                        char hold[20];

                                            for (i = 0; i < size; i++)          //just seeing if the path is parsed correctly
                                            {
                                             	printf("array before added command is: %s\n", array[i]);
                                            }

                                            for (i = 0; i < size; i++)
                                            {
                                             	strcpy(hold,array[i]);          //gets parsed directory from array
                                                strcat(hold, attach);             //adds /command to path
						 //check if hold exists here
                                                if(access(hold,F_OK) == 0)	//checks if the file exists with added \ls
                                                        printf("file exists at %s\n", hold);
                                                else
                                                        printf("file does not exist at %s\n", hold);
                                            }


				}




				/*else if(CommandValid(tokens->items[i+1]) && tokens->size > 1)		//ls command, path argument
				{
					printf("is working and is path\n");
					strcpy(commandsec,tokens->items[i]);
					strcpy(commandpath, getenv("PATH"));

					char str1[3] = "/ls";
						
					//parse path by delimiters
					char delim[] = ":";
					char *ptr = strtok(commandpath, delim);
                                        printf("%s\n", ptr);
					
					int i;
					i = 0;
					int size;
					size = 0;
					char *array[50];
					

					//printing separated directories
					while(ptr != NULL)			//putting parsed directories into array
					{
						array[i++] = ptr;		
						printf("'%s'\n", ptr);
						ptr = strtok(NULL, delim);
						size++;
					}

					char hold[20];

					    for (i = 0; i < size; i++)		//just seeing if the path is parsed correctly
					    {
        					printf("array before added command is: %s\n", array[i]);
					    }

					    for (i = 0; i < size; i++)
					    {
						strcpy(hold,array[i]);		//gets parsed directory from array
						strcat(hold, str1);		//adds /ls to path
						//check if hold exists here
                                                if(access(hold,F_OK) == 0)	//checks if the file exists with added \ls
                                                	printf("file exists at %s\n", hold);
                                        	else
                                                	printf("file does not exist at %s\n", hold);
					    }
			
				}*/
				else						//error checking for invalid input with ls
				{
					printf("invalid input\n");
				}
                
















				// // /*PART 5 -$PATH search for execution*/
				// if(CommandValid(tokens->items[i]))
				// {
						// strcpy(commandsec,tokens->items[i]);
						// strcpy(commandpath, getenv("PATH"));
						// char *temptr = strstr(commandpath, commandsec);

						// if(temptr != NULL)
						// {
								// /*pid_t waitpid(
										// pid_t pid, 
										// int *stat_loc, 
										// int options
										// );*/
								// int pid = fork();

								// if(pid == 0)
								// {
										// char *const argv[] = {/usr/bin/tokens->items[i], "-a", NULL};
										// execv("ls", argv);
										// exit(0);
								// }
								// else
								// {
										// //if(waitpid(pid, NULL, 0) == -1)
												
								// }
								// printf("%s is in path\n", tokens->items[i]);
						// }
						// else
						// {
								// printf("%s", tokens->items[i]);
								// PrintInvalid();
						// }
						// // //Command valid should SEARCH
						// //begin EXECUTING command (new process)?
				// }
		free(input);
		free_tokens(tokens);
	}

	return 0;
}

bool IsPath(char *str)
{
	if(str[0] == '/')
		return true;
	else
		return false;
}

bool FileInDir(char *str)					//If command is a file in one of the $PATH
{											// directories, run it. Else print error.
	char *directories=(char*)malloc(sizeof(char)*(strlen(getenv("PATH"))));
	strcpy(directories, getenv("PATH"));	//Creates str of all directories in PATH.
	
	
	if(1)
	{	//command runs
	}
	else
		printf("%s: Command not found.");
}

bool IsVar(char *item)
{               
        if(item[0] == '$')
                return true;
        else
                return false;
}

bool IsTilde(char *item)
{
        if(item[0] == '~')
                return true;
        else
                return false;
}

bool CommandValid(char *item)
{
        //params may need to be adjusted? **fixed
        if(item[0] == '/')
                return true;
        else
                return false;
        //loop through available directiories in Path to search for the command.
        //if exists return true.
        //else return false.
}

void PrintInvalid()
{
        printf(": Command not found.\n");
}

bool VarIsValid(char * item)
{
        //IF IsVar() returned true in the tokenizing loop, then we check here
        // if that variable is valid...
        //Must scan list of variables for a match, or else it will segfault on getenv(commando)...
        //^Comment block(s) will be removed once fixed.
        
        //while(item isnt found in env list){
        //      compare to line up until an = is encountered
        // if found return true
        //}
        //return false;
        
        return true;
}

tokenlist *new_tokenlist(void)
{
       tokenlist *tokens = (tokenlist *) malloc(sizeof(tokenlist));
	   tokens->size = 0;
	   tokens->items = (char **) malloc(sizeof(char *));
	   tokens->items[0] = NULL; /* make NULL terminated */
	   return tokens;
}

void add_token(tokenlist *tokens, char *item)
{
        int i = tokens->size;

        tokens->items = (char **) realloc(tokens->items, (i + 2) * sizeof(char *));
        tokens->items[i] = (char *) malloc(strlen(item) + 1);
		tokens->items[i + 1] = NULL;
        strcpy(tokens->items[i], item);

        tokens->size += 1;
}

char *get_input(void)
{
        char *buffer = NULL;
        int bufsize = 0;

        char line[5];
        while (fgets(line, 5, stdin) != NULL) {
                int addby = 0;
                char *newln = strchr(line, '\n');
                if (newln != NULL)
                        addby = newln - line;
                else
                        addby = 5 - 1;

                buffer = (char *) realloc(buffer, bufsize + addby);
                memcpy(&buffer[bufsize], line, addby);
                bufsize += addby;

                if (newln != NULL)
                        break;
        }

        buffer = (char *) realloc(buffer, bufsize + 1);
        buffer[bufsize] = 0;

        return buffer;
}

tokenlist *get_tokens(char *input)
{
        char *buf = (char *) malloc(strlen(input) + 1);
        strcpy(buf, input);

        tokenlist *tokens = new_tokenlist();

        char *tok = strtok(buf, " ");
        while (tok != NULL) {
                add_token(tokens, tok);
                tok = strtok(NULL, " ");
        }

        free(buf);
        return tokens;
}

void free_tokens(tokenlist *tokens)
{
        for (int i = 0; i < tokens->size; i++)
                free(tokens->items[i]);

        free(tokens);
}
