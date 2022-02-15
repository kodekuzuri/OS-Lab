/*************************
 * BHULNA MAT 
 SM->n was supposed to represent current size of Queue.
 Easily replaceable by SM->Q.size()


 my idea : for every matrix inserted, keep 2 indices i,j maintained in its structure
           we need 8 workers for multiplication. Insert one empty matrix C at the end of
           the queue when i1 = j1 = i2 = j2 = -1 , and then check the first two matrices in the queue. If the worker finds that 
           i1 = j1 = i2 = j2 = N then stop, pop the first 2 matrices and proceed

           else find D(i1)(j1)(j2) and add this result to the relevant quarter in the empty matrix 
           had inserted at the start 
 * **********************/

#include <iostream>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/wait.h>
#include<sys/types.h>

using namespace std ; 

#define N 1000
#define M 500
#define QN 8

typedef struct matrixmul{
    int status;
    int i,j,k;  // Useful in calculation of status of matrix multiplication
    int M[N][N] ; 
    int mid ; 
    int pno ;   
}matrixmul ;



struct FiniteQueue {
    int front = 0;
    int  capacity = QN;
    matrixmul queue[capacity];
    int size = 0;
 
   
    void insert(matrixmul data)
    {
        // check queue is full or not
        if (capacity == size) {
            printf("\nQueue is full\n");
            return;
        }
 
        // insert element at the rear
        else {
            queue[size] = data;
            size++;
        }
        return;
    }
 
    
    void remove()
    {
        // if queue is empty
        if (front == size) {
            printf("\nQueue is  empty\n");
            return;
        }
 
        /
        else {
            for (int i = 0; i < size - 1; i++) {
                queue[i] = queue[i + 1];
            }
 
            size--;
        }
        return;
    }
 
    // print queue elements
    void queueDisplay()
    {
        int i;
        if (front == size) {
            printf("\nQueue is Empty\n");
            return;
        }
 
        // traverse front to rear and print elements
        for (i = front; i < size; i++) {
            printf(" %d <-- ", queue[i]);
        }
        return;
    }
 
    matrixmul first_matrix()
    {
         if (front == size) {
            printf("\nQueue is  empty\n");
            return;
        }

        return queue[front];
 
    }
    matrixmul second_matrix()
    {
        if((front+1)==size)
        {
            printf("\n Queue is of size 1 \n");
            return;
        }
        return queue[front + 1];
    }

};
 

// Mno : no. of matrices to be multiplied
// job_created : no. of jobs created till now
//  
typedef struct SM{
    int job_done ;
    int job_created ; 
    FiniteQueue Q ; 
    int Mno ; 
}SM ; 


matrixmul mcreate(int i){
    matrixmul m ; 
    m.pno = i + 1 ; 
    m.status = 0 ; 
    m.i = 0 ;
    m.j = 0 ;
    m.k = 0; 

    if(i==-1)
    {
     for(int i = 0 ; i < N ; i++)
        for(int j = 0 ; j < N ; j++) m.M[i][j] = 0 ; 
   
    }
    else
    {
    for(int i = 0 ; i < N ; i++)
        for(int j = 0 ; j < N ; j++) m.M[i][j] = (rand()%19) - 9 ; 
    }
    m.mid = rand()%100000 + 1 ; 

    return m ;  
}

void createWorker(SM* sm, int NP, int NW, int i){
    int seed = time(0) + i + 2 + NP ;
    srand(seed) ; 


    while(sm->job_done < sm-> Mno){
        int sleepdur = rand()%4 ; 
        sleep(sleepdur) ;   
        
        if(sm->Q.size < 2)
        {
            continue;
        }

        matrixmul m1 = sm->Q.first_matrix();
        matrixmul m2 = sm->Q.second_matrix(); 

        if(m1.status==0)
        {
            matrixmul resultant_matrix = mcreate(-1); 
            sm->Q.insert(resultant_matrix);
            int block[M][M];


        }
        else if(m1.status==7)
        {

        }
        else
        {

        }
        
    }
    return ;
} 


void createProducer(SM* sm,  int i){
    int seed = time(0) + i + 2 ;
    srand(seed) ; 

  
    while((sm->job_created < sm->Mno)){
        // matrix multiplication block 
        matrixmul m  = mcreate(i) ;

        int sleepdur = rand()%4 ; 
        sleep(sleepdur) ;  


        if(sm->Q.size < QN){
            sm->Q.insert(m);

            (sm->job_created)++ ; 
        1}
        else if(sm->Q.size==QN) 
        {
            //printing job details 
            cout << "Generated Job data : \n" ;
            cout << "Matrix ID: " << m->mid << "\n" ; 
            cout << "producer No.: " << m->pno << "\n" ; 
            //cout << " i : " << m->i << " j : " << m->j << " k : " << m->k << "\n" ; 

            while(1)
            {
                if(sm->Q.size<QN)
                {
                     sm->Q.insert(m);
                    sm->status = 0;
                    (sm->job_created)++ ;
                    break; 
                }
            }
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

    int num_matrices ; 
    cout << "Enter number of matrices to be multiplied : " ; 
    cin >> num_matrices ; 

    // storing start time to calculate total time taken 
    time_t s = time(0) ;

    // ftok : key generation 
    key_t val = ftok("/dev/random",'c');
    
    // Shared Memory ID creation and checking allocation 
    size_t shared_memory_size = sizeof(SM);
    int smi ;  
    if( ( smi = shmget(val, shared_memory_size, 0666 | IPC_CREAT) ) < 0 ){
        perror("ERROR : ") ; 
        return -1 ; 
    }

    // shared memory creation
    // NULL : indicates shared memory segment to be attached to the first pointer allocated by 
    // the system
    // 0 : read/write permission

    SM* Smem = (SM*)shmat(smi, NULL, 0); ; 
    Smem->job_created = 0 ; 
    Smem->Mno = num_matrices ; 
    Smem->job_done = 0;

    // process IDs for processes , workers 
    pid_t producers[NP], workers[NW] ;  

    int k = 0 ;

    while(k < NP){

        // temporary PID for operations
        pid_t tpid = fork() ; 

        // child process; producer creation
        if(!tpid){
            createProducer(Smem,k) ;
            k++ ;
            shmdt(Smem);
            exit(0);
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
            createWorker(Smem, NP, NW, k) ;
            k++ ;
            shmdt(Smem);
            exit(0);
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

    // killing using process ids stored during worker and producer creation 
    for(;;){
        // condition termination
        if((Smem->job_created == num_matrices) && (Smem->Q.size() == 1)){
            int total_time = s - time(0) ;
            cout << "Time taken = " << total_time << " s\n" ; 

            int f = 0 ; 

            while(f < NP) kill(producers[f++], SIGTERM) ; 
            
            f = 0 ;
            while(f < NW) kill(workers[f++], SIGTERM) ; 

            break ;
        }
    }

    //detach shared memory 
    shmdt(Smem) ;
    
    // destruction marking 
    shmctl(Smem, IPC_RMID, 0) ;

    return 0 ; 
}
