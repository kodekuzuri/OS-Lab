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

int SZE = 0 ;

void MultiWatch(char* line){

    int i = 0 ; 
    int j = 0 ; 
    char adel[1000] ; 
    char** pipe_commands=(char**)malloc(sizeof(char*)*BUFFER);
    char leftb = '[' , nl = '\0', rightb = ']', dq = '\"', comma  = ',', pipe = '|'; 
    int num_pipe_processes = 0;
    for(;line[i] != leftb;){
        i++ ; 
    }   

    for(;line[i] != nl; i++){

        if((line[i] == rightb)||(line[i] == dq)) ; 

        else if( line[i] == comma ){
            adel[j] = pipe ; 
            j++ ; 
        }

        else {
            adel[j] = line[i] ; 
            j++ ; 
        }
    }

    // marking end of string 
    adel[j] = '\0' ;

    num_pipe_processes = pipeSplit(adel,pipe_commands); 
    int inotify_fd = inotify_init() ;

    // if(inotify_fd < 0){
    //     printf("ERROR: Error in inotify_init\n") ; 
    //     exit(0) ; 
    // }

    int add_watch_out = inotify_add_watch(inotify_fd, ".", IN_MODIFY | IN_CREATE | IN_ACCESS);
    // if ( add_watch_out < 0)
    // {
    //     printf("ERROR: Error in inotify add watch \n");
    //     exit(0);
    // }

    int current_proc = 0;
    char** arguments;
    arguments = (char**)malloc(sizeof(char*)*BUFFER);

    struct temp_files_multiwatch process_files[num_pipe_processes];
    for (int i = 0; i < num_pipe_processes; ++i)
    {
    	/* code */
    	int num_args = split_line(arguments,pipe_commands[i]);
    	int processID = fork();

    	if(processID==0)
    	{
    		char filename[300];
    		char pid_string[200];
    		CONVERT_TO_STRING("%d",getpid(),pid_string);
    		 strcat(filename,".tmp.");
    		 strcat(filename,pid_string);
    		 strcat(filename,".txt");
            int wr = open(filename, O_WRONLY | O_CREAT | O_APPEND | O_TRUNC, 0666);

            dup2(wr, STDOUT_FILENO);
            close(wr);

            setpgid(0, current_proc);
            
            
            int check_execvp = execvp(arguments[0], arguments);
             if(checkexec == -1)
      	  	{
            fprintf(stderr,"ERROR:command execution failure\n");
            exit(EXIT_FAILURE);
        	}
    	}
    	if(current_proc==0)
    	{
    		current_proc = processID;
    	}

    	char filename[300];
    	char pid_string[200];
    	CONVERT_TO_STRING("%d",processID,pid_string);
    	strcat(filename,".tmp.");
    	strcat(filename,pid_string);
    	strcat(filename,".txt");
        int rfd = open(filename, O_RDONLY | O_CREAT, 0666);


        process_files[i].filename = filename;
        process_files[i].file_desc = rfd;
        process_files[i].command = pipe_commands[i];

    }

    int ctr = 0 ; 

    while(ctr < num_pipe_processes){
        // printfunc()
        ctr++ ; 
    }

	int flag = 1;
    struct temp_files_multiwatch temp;

    for(;;){
        char filestr[1000] ; 

        int fread = read(inotify_fd, filestr, 1000) ; 

        if(!(fread + 1)){
            flag = 0 ;
            // killing process 
            kill(current_proc, SIGKILL) ;

            for(int k = 0 ; k < num_pipe_processes ; k++){
                remove(process_files[k].filename); 
            }
            break  ;
        }

        for(int j = 0 ; j < fread ;){
            struct inotify_event *e = (struct inotify_event *)&filestr[j] ;
            
            for(int k = 0 ; k < num_pipe_processes ; k++){
                if(strcmp(process_files[k].filename, (e->name)) == 0){
                    //printfunc(process_files[k].filename, process_files[k].file_desc) ; 
                }
            }

            j += e->len ; 
            j += SZE ; 
        }

    }    
}
