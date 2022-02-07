#include <stdio.h> 
#include <string.h> 
#include <stdlib.h>
#include <dirent.h>

#define MAX 100

bool prefix(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}

char* autocomplete(char* filename){

    /*********EXTRACTING FILENAMES***********/

    DIR *d;
    int n = 0 ; // count of matches 
    struct dirent *dir;
    char match [MAX][300];

    for(int i = 0 ; i < MAX ; i++){

    }

    char *check  = malloc(sizeof(char) * 300) ; 
    d = opendir("./");
    if (d){
        while ((dir = readdir(d)) != NULL){
            
            strcpy(check, (dir->d_name)) ; 
            if(prefix(check, filename)) strcpy(match[n++], check) ;  
            
        }
        closedir(d);
    }

    if(!n){
        return 
    }

    if(n == 1){
        return match[0] ; 
    }

    else if(n > 1){
        int index = 0 ; 
        for(int i = 0  ; i < n ; i++){
            printf("#%d : %s", i+1, match[i]) ; 
            printf("Enter index of filename chosen: ") ; 
            scanf("%d", &index) ; 

            return match[index-1] ;  
        }
    }

    else ; 

    return "\0" ; 

    /****************************************/
}

int main(){


    // TO BE ADDED TO READLINE FUNCTION 

    if(c == 9){ // 9 is ASCII value of tab 

        int j = i - 1 , k = 0 ; 
        char *icmplfname  = malloc(sizeof(char) * 300) ; 
        char *fname  = malloc(sizeof(char) * 300) ;

        while((j > -1) && (line[j] != ' ')){
            icmplfname[k++] = line[j--] ; 
        } 

        strrev(icmplfname) ; 
        fname = autocomplete(icmplfname) ; 

        k = 0 ; 

        if(fname[0] == '\0') ; 
        else{
             line[++j] = fname[k++] ; 
        }
    }

}