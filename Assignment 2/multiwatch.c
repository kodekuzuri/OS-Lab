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

void MultiWatch(char* a){

    int i = 0 ; 
    int j = 0 ; 
    char adel[2000] ; 
    char ** args = (char*)malloc(sizeof(char) * BUF) ; 
    char leftb = '[' , nl = '\0', rightb = ']', dq = '\"', comma  = ',', pipe = '|'; 

    for(;a[i] != leftb;){
        i++ ; 
    }   

    for(;a[i] != nl; i++){

        if((a[i] == rightb)||(a[i] == dq)) ; 

        else if( a[i] == comma ){
            adel[j] = pipe ; 
            j++ ; 
        }

        else {
            adel[j] = a[i] ; 
            j++ ; 
        }
    }

    // marking end of string 
    adel[j] = '\0' ;

    args = read_line(adel) ; 
    inotify_fd = inotify_init() ;

    if(inotify_fd < 0){
        printf("ERROR\n") ; 
        exit(0) ; 
    }

    if()



    
}
