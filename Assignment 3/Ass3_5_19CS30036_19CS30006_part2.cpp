#include <bits/stdc++.h>

using namespace std ; 

#define N 1000
#define QN 8
// structure for storing matrix multlipication results
// pno : producer no.
// mstatus : status of matrix multiplication
// M : matrix
// mid : matrix id 

typedef struct matrixmul{
    pid_t pid ; 
    //status 
    int i, j, k ;   
    int M[N][N] ; 
    int mid ; 
    int pno ;   
}matrixmul ;

typedef SM{
    int job_done ; 
    matrixmul Q[QN] ; 
    int Mno ; 
    int n ; 
    int maxn ;
    int 
}SM ; 

void createWorker(int s, pid_t pid, int NP, int NW, int i){
    int seed = time(0) + i + 2 + NP ;
    srand(seed) ; 

    SM* sm = (SM*)shmat(s, NULL, 0) ; 

    matrixmul m1, m2 ; 
    if()
    return ;
} 


void createProducer(int s, pid_t pid, int i){
    int seed = time(0) + i + 2 ;
    srand(seed) ; 

    SM* sm = (SM*)shmat(s, NULL, 0) ; 

    while((sm->job_created < sm->Mno)){
        // matrix multiplication block 
        matrixmul m  = mcreate(i, pid) ;

        int sleepdur = rand()%4 ; 
        sleep(sleepdur) ;  

        if(sm->job_created != sm->Mno){
            break ; 
        }

        if(sm->n < QN){
            cout << "Job data : \n" ;
            cout << "Matrix ID: " << sm->mid << "\n" ; 
            cout << "producer No.: " << pno << "\n" ; 
            cout << "Producer ID: " << pid  << "\n" ; 
            cout << " i : " << i << " j : " << j << " k : " << k << "\n" ; 

            sm->job_created++ ; 
        }
    }
    return ;
} 

int main(){

    int NP ; 
    cout << "Enter NP(No. of producers) : " ;
    cin >> NP ;

    int NW ; 
    cout << "Enter NW(No. of workers) : " ; 

    int Mno ; 
    cout << "Enter number of matrices to be multiplied : " ; 
    cin >> Mno ; 

    // storing start time to calculate total time taken 
    time_t s = time(0) ;

    // ftok : key generation 
    key_t val = ftok("/dev/random",'c');
    
    // Shared Memory ID creation and checking allocation 
    int smi ;  
    if( ( smi = shmget(val, sizeof(shared_memory), 0666 | IPC_CREAT) ) < 0 ){
        perror("ERROR : ") ; 
        return -1 ; 
    }

    // shared memory creation
    // NULL : indicates shared memory segment to be attached to the first pointer allocated by 
    // the system
    // 0 : read/write permission

    SM* Smem = (SM*)shmat(smi, NULL, 0); ; 
    Smem->job_created = 0 ; 
    Smem->Mno = Mno ; 
    Smem->n = 0 ; 
    Smem->job_created = 0 ;

    // process IDs for processes , workers 
    pid_t producers[NP], workers[NW] ;  

    int k = 0 ;

    while(k < NP){

        // temporary PID for operations
        pid_t tpid = fork() ; 

        // child process; producer creation
        if(!tpid){
            createProducer(Smi, getpid(), k) ;
            k++ ;
            return 0 ; 
        }

        // parent process
        else if(tpid > 0){
            // storing process id in array
            producers[k++] = tpid ;
        }

        // pid < 0 : fork error 
        else{
            k++ ;
            perror("ERROR : fork error \n") ;
            return -1 ; 
        }
    }

    k = 0 ;

    while(k < NW){

        // temporary PID for operations
        pid_t tpid = fork() ; 

        // child process; producer creation
        if(!tpid){
            createWorker(Smi, getpid(), NP, NW, k) ;
            k++ ;
            return 0 ; 
        }

        // parent process
        else if(tpid > 0){
            // storing process id in array
            workers[k++] = tpid ;
        }

        // pid < 0 : fork error 
        else{
            k++ ;
            perror("ERROR : fork error \n") ;
            return -1 ; 
        }
    }


    return 0 ; 
}

