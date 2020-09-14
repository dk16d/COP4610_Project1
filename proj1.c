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

//working functions
bool IsVar(char *item);
bool IsPath(char *item);
bool IsTilde(char *item);
bool IsDir(char *item);
bool IsInput(char *item);
bool IsOutput(char *item);
bool IsPipe(char *item);

int main()
{
	bool execute = true;                                       //Flag is set false on invalid command.
	bool runBackground = false;								   //Set true on '&' (background process).
	
	while (1) {     

//FOR PIPING/REDIRECTION, may need an if( redirecting() ) encapsulating the
// printf commandline and command-expansion loop. That way we can make use
// of the main while loop to feed one process into another...
	
		printf("%s@%s:%s> ", getenv("USER"), getenv("HOST"), getenv("PWD"));
																				   /*PART 3 - Prints user's working directory*/
		char *input = get_input();
		tokenlist *tokens = get_tokens(input);
		tokenlist *args = new_tokenlist();			//THIS IS BACKGROUND arguments.
		//printf("whole input: %s\n", input);
		
		if (strchr(input, '&') != NULL)         //& EXISTS, IS BACKGROUND PROCESS
		{
			printf("is background process\n");
			runBackground = true;
			
			int j;
			for(j = 0; j < tokens->size - 1; j += 1)
			{
				add_token(args, tokens->items[j]);
			}
		}

		for (int i = 0; i < tokens->size; i++) //Begin VARIABLE EXPANSION loop.
		{
			char commando[50];                      //Declare modded-token buffer.

			if(IsVar(tokens->items[i]))                //If token is an env variable...
			{                                                                  //Strip '$' from token.
				for(int v = 0; v < strlen(tokens->items[i]); v++)
				{                                                          //Copy it into commando.
						commando[v] = (tokens->items[i])[v+1];
				}
				
				if(getenv(commando) != NULL)   //IF variable exists...
				{                                                          //Allocate space for var expansion.
						tokens->items[i] = (char *) realloc(tokens->items[i],
								sizeof(char) * (strlen(getenv(commando)) + 1));
						strcpy(tokens->items[i], getenv(commando));
				}                                                          //Copy expanded string to token list.
				else                                               //ELSE print error, do not execute.
				{
						strcpy(tokens->items[i], commando);
						printf("%s: Undefined variable.\n");
						execute = false;
				}       
			}
																			/*PART 4 - '~' expansion*/
			if(IsTilde(tokens->items[i]))
			{                                                                  //Strip the '~'.
					char commandsec[50];              							 //(sec prevents modding of the
																			   // underlying env variable.)
					for(int v = 0; v < strlen(tokens->items[i]); v++)
					{                                                          //Copy the rest into commando.
							commando[v] = (tokens->items[i])[v+1];
					}                                                          //Copy HOME path into sec.
					strcpy(commandsec, getenv("HOME"));
																			   //Realloc space for expansion.       
					tokens->items[i] = (char *) realloc(tokens->items[i],
							sizeof(char) * (strlen(commandsec) + strlen(commando) + 1));
					strcpy(tokens->items[i], strcat(commandsec, commando));           
			}                                                                  //Concat path onto expansion
			if(IsInput(tokens->items[i]))
			{
					if( access( tokens->items[i+1], F_OK ) != -1 )
					{
							FILE *ip;
							char temp[300];
							ip = fopen(tokens->items[i+1], "r");
							fgets(temp, 300, (FILE*)ip);
							char address[300];
							strcpy(address, "/usr/bin/");
							strcpy(address, tokens->items[i+1]);
							char *myargs [] = {address, temp};
							int pid = fork();
							if(pid == 0)
								 execv(myargs[0], myargs);
							else
									waitpid(pid, NULL, 0);
					}
					else
					{
							printf("file doesn't exist");
					}
			}
			if(IsOutput(tokens->items[i]))
			{
					if( access( tokens->items[i+1], F_OK ) == -1 )
					{
							char *myargs [] = {"/usr/bin/touch", tokens->items[i+1]};
							int pid = fork();                              
							if(pid == 0)                                   
									execv(myargs[0], myargs);
							else                                                 
									waitpid(pid, NULL, 0);
					}
					if(tokens->size == 4)
					{
							FILE *op;
							op = fopen(tokens->items[i+1], "w+");
							fprintf(op, "%s", tokens->items[i-1]);
							fclose(op);
					}
					else
					{
							char word[300];
							for(int y = 0; y < (tokens->size - 3); y++)
							{
									strcat(word, tokens->items[y+1]);
									strcat(word, " ");
							}
							FILE *op;
							op = fopen(tokens->items[i+1], "w+");
							fprintf(op, "%s", word);
							fclose(op);
					}
			}
			if (IsPipe(tokens->items[i]))
			{
					
			}
							   // and copy to token list.
	//      printf("ToKeN %d: (%s)\n", i, tokens->items[i]);
		}

		if(!runBackground && execute && !IsDir(tokens->items[0])) //If no token errors yet...
		{                                                                               //Begin COMMAND EXPANSION.
				char *dirPaths = (char *) malloc(sizeof(char) * (strlen(getenv("PATH")) + 1));
				strcpy(dirPaths, getenv("PATH"));       //Make safe copy of paths.
				char *path = strtok(dirPaths, ":"); //Assign first possible path.
				char cmd[16];                                           //Temporary store token[0]
				strncpy(cmd, tokens->items[0], 15); // into cmd.
				bool pathFound = false;
				
				while(path != NULL)                                     //Check paths until cmd is found.
				{                                                                       //Temporarily save command token[0].
						tokens->items[0] = (char *) realloc(tokens->items[0],
								sizeof(char) * (strlen(path) + strlen(cmd) + 2));
						strcpy(tokens->items[0], path); //After alloc'ing for concatenation,
						strcat(tokens->items[0], "/");  // concatenate "/cmd" onto token.
						strcat(tokens->items[0], cmd);
				//      printf("CONCAT'D PATH IS: %s\n", tokens->items[0]);
						
						if(access(tokens->items[0], F_OK) == 0)         
						{                                                               //IF command is found in path,
								pathFound = true;                       // no more seaching needed!
				//              printf("file exists at: %s\n", path);
								break;
						}
						else                                                    //Else, not yet found, assign
								path = strtok(NULL, ":");       // next path and try again.
				}
				
				if(pathFound)                                           //If found, execute it!
				{                                                                       //Create child process. Since there
						int pid = fork();                               // is now two processes running...
						if(pid == 0)                                    //If child process, execute.
								execv(tokens->items[0], tokens->items);
						else                                                    //If parent (main), wait your turn.
								waitpid(pid, NULL, 0);
				}
				else                                                            //ELSE, path was never found.
						printf("%s: Command not found.\n");
				
				// if (strchr(input, '&') != NULL)         						//& EXISTS, IS BACKGROUND PROCESS
					// {
						// printf("is background process");
						// for (int j = 0; j < 10; j++)
						// {
							// if (parray[j] == -1)
							// {
								// int pid = fork();
								// parray[j] = getpid();

								// //how do i execute background process???

								// break;
							// }
						// }
					// }



					// //while other things are executing
					// //after every prompt
					// for (int j = 0; j < 10; j++)
					// {
							// if(parray[j] == -1)     //if -1, slot is empty don't do anything
									// break;
							// else{
								// int status = waitpid(parray[j], NULL, WNOHANG);

							// //check if child is finished or if its still running
							// if (status != 0)        //tells if process is finished or not
							// {

									// //print - process is finished
									// printf("[%d]+ Done\t\t", j+1);
									// printf("%s\t\t", input);            //PRINT WHOLE COMMAND LINE NEED TO GET RID OF &
							// }
							// else //process is still running
							// {
									// //do nothing
							// }
							// }
					// }//end while loop
			free(dirPaths);
		}
		runBackground = false;						//Reset background process flag!
		free(args);									//Free the background arguments.
		free(input);
		free_tokens(tokens);
	}

	return 0;
}

bool IsInput(char *item)
{
        if(item[0] == '<')
                return true;
        else
                return false;   
}

bool IsOutput(char *item)
{
        if(item[0] == '>')
                return true;
        else
                return false;
}

bool IsPipe(char *item)
{
        if(item[0] == '|')
                return true;
        else
                return false;
}

bool IsPath(char *item)
{
        if(item[0] == '/')
                return true;
        else
                return false;
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

bool IsDir(char *item)
{
        if(item[0] == '/')
                return true;
        else
                return false;
}

void PrintInvalid()
{
        printf(": Command not found.\n");
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
//this is just test
