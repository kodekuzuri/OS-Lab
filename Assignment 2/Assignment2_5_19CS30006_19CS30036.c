#include <stdio.h>
#include <stdlib.h>
#include "unistd.h"
#include "sys/wait.h"
#include "fcntl.h"
#include <string.h>

#define BUFFER 512


// Builtin functions
int builtin_help(char** args);
int builtin_exit(char** args);

char* builtin_names[] = {"help","exit"}; // names of builtin funcs

int (*builtin_functions[])(char**) = {&builtin_help,&builtin_exit}; // array of builtin funcs

int num_builtin_functions() // num of builtin funcs
{
	return sizeof(builtin_names)/sizeof(char*);
}


int builtin_help(char** args)
{
	int num_builtins = num_builtin_functions();
    printf("These are the built-in commands:\n");
    for(int i=0;i<num_builtins;i++)
    {
         printf("%s\n",builtin_names[i]);       
    }
    printf("Type man <command> to know about a command\n");
    printf("Type man to know about other commands\n");
    return EXIT_SUCCESS;
}
//exit from shell
int builtin_exit(char** args)
{
    return 1;
}
int read_line(char* line)
{
	int buffer_size = BUFFER;
	int c;
	int i = 0;

	while(1)
	{
		c = getchar();

		if(c==EOF || c=='\n')
		{
			line[i] = '\0';
			return EXIT_SUCCESS;
		}
		line[i++] = c;

		// Since we dont know how much memory would be needed for line, we keep on reallocating more memory as and when it exceeds.
		if(i >= buffer_size)
        {
            buffer_size+=BUFFER;
            line = (char*)realloc(line, buffer_size);
            if(!line)
            {
                printf("Error: memory allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
	}
}

int split_line(char** args,char* line)
{
	int num_args = 0;
	int buffer_size = BUFFER;
	int len = strlen(line);
	int arg_len = 0;

	for (int i = 0; i < BUFFER; ++i)
	{
		/* code */
		args[i] = (char*)malloc(sizeof(char)*len);
		if(args[i]==NULL)
		{
			printf("Error: memory allocation error\n");
			exit(EXIT_FAILURE);
		}
	}

	for (int i = 0; i < len; ++i)
	{
		/* code */
		if(line[i]=='\"')   // Read till next double quote
		{
			int j = i+1;
			 arg_len=0;
			while(j<len && line[j]!='\"')
			{
				args[num_args][arg_len] = line[j];
				arg_len++;
				j++; 
			}
			if(strlen(args[num_args])>0)
			{
				num_args++;
			}
			i = j;
		}
		else if(line[i]=='\'') // Read till next single quote
		{
			int j = i+1;
			 arg_len=0;
			while(j<len && line[j]!='\'')
			{
				args[num_args][arg_len] = line[j];
				arg_len++;
				j++; 
			}
			if(strlen(args[num_args])>0)
			{
				num_args++;
			}
			i = j;
		}
		else if(line[i]==' ' || line[i]=='\n' || line[i]=='\t') // White space or end line or tab means end of argument
		{
			arg_len = 0;
			if(strlen(args[num_args])>0)
			{
				num_args++;
			}
		}
		else
		{
			args[num_args][arg_len] = line[i];
			arg_len++;

			if(i == len-1)
			{
				if(strlen(args[num_args])>0)
				{
				num_args++;
				}
			}
		}

		// Since we dont know how much memory would be needed for line, we keep on reallocating more memory as and when it exceeds.
		if(num_args >= buffer_size)
        {
            buffer_size+=BUFFER;
            args = (char**)realloc(args, sizeof(char*)*buffer_size);
            if(!args)
            {
                printf("Error: memory allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
	}
	return num_args;
}

int external_command(char** args,int num_args)
{
	return EXIT_SUCCESS;
}

int run_command(char** args,int num_args)
{
	if(!args[0])   // No command
		return EXIT_SUCCESS;

	int num_builtins = num_builtin_functions();
	for (int i = 0; i < num_builtins; ++i)
	{
		/* code */
		if(strcmp(builtin_names[i],args[0])==0)
          {
             return (*builtin_functions[i])(args); 
          }
	}
	return external_command(args,num_args);
}
void command_loop()
{
	int status;
	int num_args;
	char* line;
	char** args;

	do
		{
			/* code */
			line = (char *)malloc(sizeof(char)*BUFFER);
			status = read_line(line);
			args = (char**)malloc(sizeof(char*)*BUFFER);
			num_args = split_line(args,line);
			if(args!=NULL)
			status = run_command(args,num_args);

		} while (status==EXIT_SUCCESS);	
}

int main(int argc, char const *argv[])
{
    printf("#################Welcome to my shell!!#################\n");
	command_loop();
	return 0;
}
