#include <stdio.h>
#include <stdlib.h>
#include "unistd.h"
#include "sys/wait.h"
#include "fcntl.h"
#include <string.h>
#include <signal.h>
#include <termios.h>
#include <dirent.h>
#include <stdbool.h>

#define BUFFER 512
#define MAX 100
#define MAX_HISTORY_LENGTH 10000
#define MAX_COMMAND_LENGTH 512

char history[MAX_HISTORY_LENGTH+1][MAX_COMMAND_LENGTH];  // To read history from file to array
int current_history_length;

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

char *strrev(char *str)
{
      char *p1, *p2;

      if (! str || ! *str)
            return str;
      for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
      {
            *p1 ^= *p2;
            *p2 ^= *p1;
            *p1 ^= *p2;
      }
      return str;
}


void sig_int_handler(int sig_no)
{
	printf("\nCommand execution stopped\n");
	fflush(stdout);
}

void sig_tstp_handler(int sig_no)
{
	printf("\n Command moved to background\n");
	fflush(stdout);
}

void add_to_history(char* line)
{
	if(current_history_length<MAX_HISTORY_LENGTH)
	{
		for (int i = current_history_length; i >0; i--)
		{
			/* code */
			strcpy(history[i],history[i-1]);
		}
		current_history_length++;
		strcpy(history[0],line);
	}
	else
	{
		for (int i = current_history_length; i >0; i--)
		{
			/* code */
			strcpy(history[i],history[i-1]);
		}
		strcpy(history[0],line);
	}
}

void show_history()
{
	printf("The most recent  commands are given below: \n");
	if(current_history_length<1000)
	{
		for (int i = 0; i < current_history_length; ++i)
	{
		/* code */
		printf("%s\n",history[i] );
	}	
	}
	else
	{
	for (int i = 0; i < 1000; ++i)
	{
		/* code */
		printf("%s\n",history[i] );
	}
	}
}

int max_num(int num1, int num2)
{
    return (num1 > num2 ) ? num1 : num2;
}

int longest_substring(char* X, char* Y, int m, int n)
{
    
 
    int LCSuff[m + 1][n + 1];
    int result = 0; // To store length of the
                    // longest common substring
 
    for (int i = 0; i <= m; i++)
    {
        for (int j = 0; j <= n; j++)
        {
            
            if (i == 0 || j == 0)
                LCSuff[i][j] = 0;
 
            else if (X[i - 1] == Y[j - 1]) {
                LCSuff[i][j] = LCSuff[i - 1][j - 1] + 1;
                result = max_num(result, LCSuff[i][j]);
            }
            else
                LCSuff[i][j] = 0;
        }
    }
    return result;
}
 
void search_history()
{
	printf("Enter search term\n");
    char term[512];
    scanf("%s",term);
	int flag = 0;
	for (int i = 0; i < current_history_length; ++i)
	{
		/* code */
		if(strcmp(history[i],term)==0)
		{
			printf("Most recent command matching the prompt is Number %d and is %s\n",i+1,history[i] );
			flag = 1;
			break;
		}
	}
	if(flag)
	{
		return;
	}
	else
	{
		int max = 0;
		int ans = 0;
		char* curr_command = "";
		for (int i = 0; i < current_history_length; ++i)
		{
			/* code */
			int m = strlen(term);
			int n = strlen(history[i]);
			int curr_length = longest_substring(term,history[i],m,n);
			if(curr_length>max)
			{
				max = curr_length;
				ans = i;
				curr_command = history[i];
			}
		}
		if(max<=2)
		{
			printf("No match for search term in history\n");
		}
		else
		{
			printf("Longest substring match was found in Command number %d which is %s\n",ans,curr_command);
		}
	}
}

bool prefix(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}
/*
void autocomplete(char* filename){



    DIR *d;
    int n = 0 ; // count of matches 
    struct dirent *dir;
    char match [MAX][300];

    char *check  = malloc(sizeof(char) * 300) ; 

    d = opendir("./");

    if (d){
        //printf("\nparsing\n\n") ; 
        while ((dir = readdir(d)) != NULL){
            
            strcpy(check, (dir->d_name)) ; 
            //printf("%s\n", check) ; 
            if(prefix(filename, check)){
                strcpy(match[n++], check) ;
                //printf("matched : %s\n", check) ;  
            } 
            
        }
        closedir(d);
    }

    if(!n);

    if(n == 1){
        for(int z = )
    }

    else if(n > 1){
        int index = 0 ; 
        for(int i = 0  ; i < n ; i++)printf("#%d : %s \n", i+1, match[i]) ; 
            
        printf("Enter index of filename chosen: ") ; 
        scanf("%d", &index) ; 
        ret = match[index-1] ; 
        return ret ; 
    }

    else ; 

    return "\0" ; 

}
*/

int read_line(char* line)
{
	int buffer_size = BUFFER;
	int c;
	int i = 0;

	// termios is used to detect ctrl+R and tab characters 
	struct termios old,new;
	tcgetattr(STDIN_FILENO,&old);
	new = old;
	new.c_lflag &= ~(ICANON | ECHO);
	new.c_cc[VMIN] = 1;
	new.c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO,TCSANOW,&new);

	while(1)
	{
		c = getchar();

		if(c==EOF || c=='\n')
		{
			line[i] = '\0';
			printf("\n");
			tcsetattr(STDIN_FILENO,TCSANOW,&old);
			return EXIT_SUCCESS;
		}
		else if(c==18)   // CTRL + R check
		{
			tcsetattr(STDIN_FILENO,TCSANOW,&old);
			search_history();
			return EXIT_SUCCESS;
		}
		else if(c==127) // Backspace check
		{
			if(i>0)
			{
				printf("\b \b");
				line[i] = '\0';
				i--;
			}
			else
				continue;
		}
		else if( c== 9 ) // Tab check
		{
            tcsetattr(STDIN_FILENO,TCSANOW,&old) ;

            int j = i - 1 , k = 0 ; 

            char *icmplfname  = malloc(sizeof(char) * 300) ; 
            char *fname  = malloc(sizeof(char) * 300) ;

            while((j > -1) && (line[j] != ' ')){
                icmplfname[k++] = line[j--] ; 
            } 
            //k-- ; 

            strrev(icmplfname) ; 

			if((icmplfname[0] == '.') && (icmplfname[1] == '/')){
				for(int i = 0 ; i < (k - 2) ; i++) icmplfname[i] = icmplfname[i+2] ; 
				icmplfname[k-2] = '\0' ; 
				k = k - 2 ;
			}

            //printf("\nfilename to be completed: %s\n", icmplfname) ; 
			printf("\nline : %s\n", line) ; 

            DIR *d;
            int n = 0 ; // count of matches 
            struct dirent *dir;
            char match [MAX][300];

            char *check  = malloc(sizeof(char) * 300) ; 

            d = opendir("./");

            if (d){
                //printf("\nparsing\n\n") ; 
                while ((dir = readdir(d)) != NULL){
                    
                    strcpy(check, (dir->d_name)) ; 
                    //printf("%s\n", check) ; 
                    if(prefix(icmplfname, check)){
                        strcpy(match[n++], check) ;
                        //printf("matched : %s\n", check) ;  
                    } 
                }
                closedir(d);
            }

           // printf("\nn = %d\n", n) ;

            int index = 0 ;

            if(!n) ; 

            else if(n == 1){
                do{
                    line[i++] = match[0][k] ; 
                    printf("%c",line[i-1]) ;
                    k++ ;  
                }while(match[0][k] != '\0') ; 
            }

            else{
                printf("\n") ; 
                for(int y = 0 ; y < n ; y++){
                    printf("#%d: %s\n", (y+1), match[y]) ; 
                }

                printf("Enter index of filename selected: ") ; 
                scanf("%d", &index) ; 

				int f = 0  ;

                for(; f < i ; f++) printf("%c",line[f]) ; 
                do{
                    line[i++] = match[index-1][k] ; 
                    printf("%c",line[i-1]) ;
                    k++ ;  
                }while(match[index-1][k] != '\0') ;
            }

            continue ;
		}
		else
		{
		line[i++] = c;
		printf("%c",c);
		} 
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
            while((WIFSIGNALED(s) == 0) && (WIFEXITED(s) == 0)){
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

    	// For history command 
    	 if(strcmp(arguments[0],"history")==0)
    	 {
    	 	show_history();
    	 	return EXIT_SUCCESS;
    	 }
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
        return external_command(arguments,0, 1, n) ; 
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

			signal(SIGINT,sig_int_handler);
			signal(SIGTSTP,sig_tstp_handler);
			line = (char *)malloc(sizeof(char)*BUFFER);
			status = read_line(line);
			int l = strlen(line);
			if(l==0)
				continue;
			args = (char**)malloc(sizeof(char*)*BUFFER);
			num_args = split_line(args,line);
			if(args[0]==NULL)
				continue;
			if(args!=NULL)
			{
				add_to_history(line);
				status = run_command(args,num_args);
			}
			free(line);
			free(args);
			fflush(stdin);
       	    fflush(stdout);
		} while (status==EXIT_SUCCESS);	
}

int main(int argc, char const *argv[])
{
	char *filename = "history.txt";
	int pos = 0;
	FILE *fptr = fopen(filename,"r");
	if(fptr)
	{
		// fseek(fptr,0,SEEK_END);
		// long size = ftell(fptr);
		// printf("%ld\n",size );
	while(fgets(history[pos],MAX_COMMAND_LENGTH,fptr))
	{
		history[pos][strlen(history[pos])-1] = '\0';
		pos++;
	}
	current_history_length = pos;
	fclose(fptr);
	}
    printf("#################Welcome to my shell!!#################\n");
    command_loop();
    FILE *fptr2 = fopen(filename,"w");
    for (int i = 0; i < current_history_length; ++i)
    {
    	/* code */
    	fprintf(fptr2,"%s\n",history[i]);
    }
    fclose(fptr2);
	return 0;
}
