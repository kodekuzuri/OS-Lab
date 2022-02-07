char** pipeSplit(char** arr, int *p)
{

    // VARIABLE INITIALIZATION

    *p = 0 ; 
    int i = 0, l = 0, k = 0 ;
    int bsize = BUFFER ; 
    int n = strlen(arr) ;  
    char** args = (char**)malloc(sizeof(char*) * bsize) ;
    
    // END 


    // MEMORY ALLOCATION CHECK 

    if(args==NULL){
		printf("Error: memory allocation error\n");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < BUFFER; ++i){
		args[i] = (char*)malloc(sizeof(char) * n);
		if(args[i]==NULL)
		{
			printf("Error: memory allocation error\n");
			exit(EXIT_FAILURE);
		}
	}

    // MEMORY ALLOCATION CHECK ENDS

    // BODY

    while(i < n){

        if((arr[i] == '\'') || (arr[i] == '\"')){
            char ch = arr[i] ; 

            args[k][l++] = arr[i++] ; 

            for(;(i < n) && (arr[i] != ch);){
                args[k][l++] = arr[i++] ; 
            }

            args[k][l++] = arr[i++] ; 
        }

        else if(arr[i] == '|'){
            int ll = strlen(arr[k]) ; 

            if(ll <= 0){
                *p = 0 ; 
                fprintf(stderr,"SYNTAX ERROR \n") ;
                return NULL;
            } 

            k++ ; 
            l = 0 ; 
            i++ ; 

            if(k >= bsize){
                bsize += BUFFER ;
                args = (char**)realloc(args, sizeof(char*) * bsize);
                if(!args)
                {
                    printf("ERROR: memory allocation error\n");
                    exit(EXIT_FAILURE);
                }
            }
        }

        else{
            args[k][l++] = arr[i++] ; 
        }
    }


    *p = k + 1 ; 
    args[p] = NULL ;  
    return args ; 
}