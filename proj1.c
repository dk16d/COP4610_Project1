//#THIS IS A TEST FOR OUR WORKBRANCH & LINPROG 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

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

bool list(char *item);
bool IsVar(char *item);
bool VarIsValid(char * item);
bool IsTilde(char *item);
bool CommandValid(char *commando);

int main()
{
        while (1) {
                /*PART 3 - Prompts user with working directory*/
                printf("%s@%s:%s> ", getenv("USER"), getenv("HOST"), getenv("PWD"));
        
                /* input contains the whole command
                 * tokens contains substrings from input split by spaces
                 */
        
                char *input = get_input();
                //printf("whole input: %s\n", input);
                tokenlist *tokens = get_tokens(input);
                for (int i = 0; i < tokens->size; i++) 
                {
			char commando[50] = " ";
			char commandsec[50] = " "; 
			char commandpath[150] = " ";
                        //printf("token %d: (%s)\n", i, tokens->items[i]);
                        /*this is for part 1 and 2*/
						
			//NOTE: printf("%s\n", getenv(commando)); vvBELOWvv is problematic.
			//  if token variable is invalid, we get a seg fault!
			// This will be removed once fixed in part5, checking for valid paths...
						
                        if(IsVar(tokens->items[i]))
                        {
                                strcpy(commandsec,tokens->items[i]);
                                for(int v = 0; v < strlen(commandsec); v++)
                                {
                                        commando[v] = commandsec[v+1];
                                }
								
		//Must scan list of variables right here or else segfault on getenv(commando)...
		//Look at func defintion of VarIsValid() for details.
		//Perhaps create a variable for valid/not valid. If valid, execute, if not, display error msg.
		//^Temporary Comment block(s) will be removed once fixed.
								
                                printf("%s\n", getenv(commando));
                        //printf("\"%s\" NEEDS TO BE EXPANDED\n", tokens->items[i]);
                        //TESTING MODDED FILE
                        }
                        /*PART 4 - Tilde detection and expansion*/
                        if(IsTilde(tokens->items[i]))
                        {
                                strcpy(commandsec,tokens->items[i]);
                                for(int v = 0; v < strlen(commandsec); v++)
                                {
                                        commando[v] = commandsec[v+1];
                                }
                                if(strlen(commando) == 0)
                                        printf("%s\n", getenv("HOME"));
                                else
                                        printf("%s%s\n", getenv("HOME"), commando);
                        }
			

// /*PART 5 -$PATH search for execution*/

			if (list(tokens->items[i]))	//check if this is a ls command
			{
				printf("registered as list command\n");
				if (tokens->size == 1)				//ls is the only command, no arguments
				{
					printf("ls but doesn't specify a path\n");
				}
				else if(CommandValid(tokens->items[i+1]) && tokens->size > 1)		//ls command, path argument
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
			
				}
				else						//error checking for invalid input with ls
				{
					printf("invalid input\n");
				}
			}
                }
                free(input);
                free_tokens(tokens);
        }

        return 0;
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

bool IsVar(char *item)
{		
        if(item[0] == '$')
                return true;
        else
                return false;
}

bool VarIsValid(char * item)
{
	//IF IsVar() returned true in the tokenizing loop, then we check here
	// if that variable is valid...
	//Must scan list of variables for a match, or else it will segfault on getenv(commando)...
	//^Comment block(s) will be removed once fixed.
	
	//while(item isnt found in env list){
	//	compare to line up until an = is encountered
	// if found return true
	//}
	//return false;
	
	return true;
}

bool IsTilde(char *item)
{
        if(item[0] == '~')
                return true;
        else
                return false;
}

bool list(char *item)
{
	if(item[0] == 'l' && item[1] == 's')
		return true;
	else
		return false;
}

tokenlist *new_tokenlist(void)
{
        tokenlist *tokens = (tokenlist *) malloc(sizeof(tokenlist));
        tokens->size = 0;
        tokens->items = NULL;
        return tokens;
}

void add_token(tokenlist *tokens, char *item)
{
        int i = tokens->size;

        tokens->items = (char **) realloc(tokens->items, (i + 1) * sizeof(char *));
        tokens->items[i] = (char *) malloc(strlen(item) + 1);
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
