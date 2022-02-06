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
    int vis = 0 ; 
    *p = 0 ; 

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

        else if(line[i] == '|'){
            vis  = 1 ; 
            int l = strlen(args[num_args]) ; 

            if(l <= 0){
                *p = 0 ;
                fprintf(stderr, "ERROR: Syntax error\n") ; 
                return 0 ; 
            }

            arg_len = 0 ;
            num_args++ ; 

            /*
            if(num_args >= buffer_size){
                buffer_size+=BUFFER;
                args = (char**)realloc(args, sizeof(char*)*buffer_size);
                if(!args)
                {
                    printf("Error: memory allocation error\n");
                    exit(EXIT_FAILURE);
                }
            }
            */             
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

    if(vis){
        *p = num_args + 1 ; 
        args[p] = NULL ; 
    }
	return num_args;
}

int external_command(char** arguments,int fdIN, int fdOUT, int n)
{
    // arguments : stores all the arguments entered 
    // n : the number of arguments passed
    // fdIN, fdOUT: file descriptors for input/output respectively 

    // function body 
	
    pid_t wait_PID ; // process ID created to handle wait (& commands)
    int s ; // status variable for wait handling  
    pid_t processID = fork() ; // process ID to store status of forked process

    // parent process
    if(processID > 0){
        // if & is present, do not wait for child process to complete and let it execute in the background
        if(!strcmp(arguments[n-1], "&")) ; 
        
        // wait for other commands 
        else {
            wait_PID = waitpid(processID, &s, WUNTRACED) ;

            // WIFSIGNALED: error signal
            // WEXITSTATUS: exit status (normally/abnormally)

            // keep waiting until process hasn't exited and is behaving normally 
            while((WIFSIGNALED(s) == 0) && (WEXITSTATUS(s) == 0)){
                wait_PID = waitpid(processID, &s, WUNTRACED) ;
            }
        }
    }

    //child process
    else if(!processID){

        // flag to check symbol commands (<,>,&), lenC to store the length of commands 
        int flag = 0 , lenC = 0, k = 0, i = 0 ; 
        
        // stdout redirected to fdOUT
        if(fdOUT != 1){
            dup2(fdOUT, 1) ; 
            close(fdOUT) ; 
        }

        // stdin redirected to fdIN
        if(fdIN != 0){
            dup2(fdIN, 0) ; 
            close(fdIN) ; 
        }

        while(k < n){

            char *ch  ; 
            ch  = arguments[k] ;
            if(flag==0)
            { 
            if(((!strcmp(ch, ">")) || (!strcmp(ch, "<")) || (!strcmp(ch, "&"))) && (flag == 0)){
                // storing length of the first command
                lenC = k ; 
                // marking flag 
                if(flag < 1)flag++ ; 
            }
            }
            if(!strcmp(ch, "<")){
                int fdread ; 
                // file descriptor for reading file on RHS 
                fdread = open(arguments[k + 1], O_RDONLY) ;
                // redirecting stdin to fdread
                dup2(fdread, 0) ;  
            }

            if(!strcmp(ch, ">")){
                int fdwrite ; 
                // file descriptor for writing in file on RHS 
                fdwrite  = open(arguments[k + 1], O_CREAT | O_WRONLY | O_TRUNC , 0666) ; 
                // redirecting stdout to fdwrite 
                dup2(fdwrite, 1) ; 
            }

            // in case of &, we don't need to wait or do anything and just execute the next command

            k++ ; 
        }

        // if no symbol command found, then whole string is a single command
        if(flag == 0) lenC = n ; 

        char** arg = (char**)malloc(sizeof(char*)*(lenC + 1)) ; 
                while(i < lenC){
            arg[i] = arguments[i] ; 
            i++ ; 
            if(i == lenC) arg[i] = NULL ; 
        }
        int checkexec = execvp(arg[0], arg) ; 

        if(checkexec == -1)
        {
            fprintf(stderr,"ERROR:command execution failure\n");
            exit(EXIT_FAILURE);
        }

    }

    // processID < 0 => forking error 
    else{ 
        fprintf(stderr, "ERROR: fork failure\n") ; 
        exit(EXIT_FAILURE) ; 
    }


    // function end 

	return EXIT_SUCCESS;
}

int run_command(char** arguments, int n){
    // arguments : stores all the arguments entered 
    // n : the number of arguments passed [used in the case of non-in-built commands are entered]
    
    // if arguments are presents, execute 
    if(arguments[0] != NULL){
        int i = 0 ; 
        int num_builtins = num_builtin_functions() ; 

        while(i < num_builtins){
            // in case of match 
            if(strcmp(arguments[0], builtin_names[i]) == 0){
                return (*builtin_functions[i])(arguments) ;
            }
            i++ ; 
        }

        // In case of arguments not matching in-built functions, call external_command
        return external_command(arguments,0,1, n) ; 
    }

    // In Case of no arguments, exit 
    return EXIT_SUCCESS ; 
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

			free(line);
			free(args);

		} while (status==EXIT_SUCCESS);	
}

int main(int argc, char const *argv[])
{
    printf("#################Welcome to my shell!!#################\n");
    printf("#################Sussy Baka Lesss gooooo#################\n") ; 	
    command_loop();
	return 0;
}
