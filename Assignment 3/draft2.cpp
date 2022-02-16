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

#define N 10
#define M 5
#define QN 8

typedef struct matrixmul{
    int status;
    //int i,j,k;  // Useful in calculation of status of matrix multiplication
    int matrix[N][N] ; 
    int resultant_index ; 
    int mid ; 
    int pno ;   
}matrixmul ;



struct FiniteQueue {
    int front;
    matrixmul queue[QN];
    int size ;

   
   
    void insert(matrixmul data)
    {
        // check queue is full or not
        if (QN == size) {
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
 
        
        else {
            for (int i = 0; i < size - 1; i++) {
                queue[i] = queue[i + 1];
            }
 
            size--;
        }
        return;
    }
 
  
    matrixmul first_matrix()
    {
       

        return queue[front];
 
    }
    matrixmul second_matrix()
    {
       
        return queue[front + 1];
    }

};
 

// Mno : no. of matrices to be multiplied
// job_created : no. of jobs created till now
//  
typedef struct SM{
    //int job_done ;
    int job_created ; 
    FiniteQueue* Q ; 
    int Mno ; 
}SM ; 


matrixmul mcreate(int i){
    matrixmul m ; 
    m.pno = i + 1 ; 
    m.status = 0 ; 
    /*
    m.i = 0 ;
    m.j = 0 ;
    m.k = 0; 
    */
    if(i==-1)
    {
     for(int i = 0 ; i < N ; i++)
        for(int j = 0 ; j < N ; j++) m.matrix[i][j] = 0 ; 
   
    }
    else
    {
    for(int i = 0 ; i < N ; i++)
        for(int j = 0 ; j < N ; j++) m.matrix[i][j] = (rand()%19) - 9 ; 
    }
    m.mid = rand()%100000 + 1 ; 

    return m ;  
}

void createWorker(SM* sm, int NP, int NW, int i){
    cout<<"BBBB"<<endl;
    int seed = time(0) + i + 2 + NP ;
    srand(seed) ; 


    //while(sm->job_done < sm-> Mno){
        cout<<"AAAAA"<<endl;
        int sleepdur = rand()%4 ; 
        sleep(sleepdur) ;   
        cout<<" Queue size: "<<sm->Q->size<<endl;
        if(sm->Q->size < 2)
        {
            return;
        }

        matrixmul m1 = sm->Q->first_matrix();
        matrixmul m2 = sm->Q->second_matrix(); 

        cout<<"STATUS M1"<<m1.status<<endl;
        // exit(0);
        if(m1.status==0)
        {
            // cout<<"Status of worker: Reading matrices for the first time"<<endl;
            // cout<<"Worker number: "<<i<<endl;
            // cout<<"Matrix id 1: "<<m1.mid<<endl;
            // cout<<"Matrix id 2: "<<m2.mid<<endl;

            //sm->job_done = 0 ; 

            matrixmul resultant_matrix = mcreate(-1); 
            int blockA[M][M],blockB[M][M],blockD[M][M];
         

            for (int i = 0; i < M; ++i)
            {
                /* code */
                for(int j=0;j<M;j++)
                {
                    blockA[i][j] = m1.matrix[i][j];
                    blockB[i][j] = m2.matrix[i][j];
                    blockD[i][j] = 0;

                    for (int k=0; k<M; ++k) blockD[i][j] += blockA[i][k] * blockB[k][j];

                }
            }

            for (int i = 0; i < M; ++i)
            {
                /* code */
                for(int j = 0; j<M; j++)
                {
                    resultant_matrix.matrix[i][j] = blockD[i][j];
                }
            }

            m1.resultant_index = sm->Q->size ;
            m2.resultant_index = sm->Q->size ;
            
            sm->Q->insert(resultant_matrix);
            
            m1.status++;
            m2.status++;

        }

        else
        {

            int I = (m1.status & 4) ? 1 : 0;
            int J = (m1.status & 2) ? 1 : 0;
            int K = (m1.status & 1) ? 1 : 0;
            int blockA[M][M],blockB[M][M],blockD[M][M];

             for (int i = M*I; i < M*(I+1); ++i)
                  {
                /* code */
                for(int j=M*J;j<M*(J+1);j++)
                    {
                    blockA[i][j] = m1.matrix[i][j];
                    blockB[i][j] = m2.matrix[i][j];
                    blockD[i][j] = 0;
                    for (int k=M*K; k<M*(K+1); ++k) blockD[i][j] += blockA[i][k] * blockB[k][j];

                    }
                  }
                  
            if(K==0){
                cout<<"Status of worker: Copying blocks"<<endl;
            cout<<"Worker number: "<<i<<endl;
            cout<<"Matrix id 1: "<<m1.mid<<endl;
            cout<<"Matrix id 2: "<<m2.mid<<endl;
                for (int i = M*I; i < M*(I+1); ++i)
                    {
                        /* code */
                        for(int j = M*J; j < M*(J+1) ; j++)
                        {
                            sm->Q->queue[m1.resultant_index].matrix[i][j] = blockD[i][j] ;
                        } 
                    }
            }

            else if(K==1)
            {
                cout<<"Status of worker: Adding blocks"<<endl;
            cout<<"Worker number: "<<i<<endl;
            cout<<"Matrix id 1: "<<m1.mid<<endl;
            cout<<"Matrix id 2: "<<m2.mid<<endl;
              for (int i = M*I; i < M*(I+1); ++i)
                    {
                        /* code */
                        for(int j = M*J; j< M*(J+1) ; j++)
                        {
                            sm->Q->queue[m1.resultant_index].matrix[i][j] += blockD[i][j] ;
                        }
                    }
            }

            if((m1.status == m2.status) && (m1.status == 7)){
                //sm->job_done = 1 ; 
                sm->Q->remove() ; 
                sm->Q->remove() ; 


            }

                m1.status++ ; 
                m2.status++ ; 

        }
        
//    }
    return ;
} 


void createProducer(SM* sm,  int i){
    int seed = time(0) + i + 2 ;
    srand(seed) ; 

  
    while(1){

        double sleepdur = ((rand()%400)/100) ; 
        sleep(sleepdur) ;  
        if((sm->job_created < sm->Mno))
        {

        
        //printf("Job created: %d\n",sm->job_created );
        // matrix multiplication block 
        matrixmul m  = mcreate(i) ;

        

        if(sm->Q->size < QN){
                 //printing job details 
                cout << "Generated Job data : \n" ;
                cout << "Matrix ID: " << m.mid << "\n" ; 
                cout << "producer No.: " << m.pno << "\n" ; 
                cout<<"Matrix : "<<m.matrix[0][0]<<endl;
                //cout << " i : " << m->i << " j : " << m->j << " k : " << m->k << "\n" ; 
            sm->Q->insert(m);
            (sm->job_created)++ ; 
        }
        else if(sm->Q->size==QN) 
        {
            
            while(1)
            {
                if(sm->Q->size<QN)
                {
                    cout<<"HERE"<<endl;
                    //printing job details 
                cout << "Generated Job data : \n" ;
                cout << "Matrix ID: " << m.mid << "\n" ; 
                cout << "producer No.: " << m.pno << "\n" ; 
                //cout << " i : " << m->i << " j : " << m->j << " k : " << m->k << "\n" ; 

                     sm->Q->insert(m);
                    (sm->job_created)++ ;
                    break; 
                }
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
    cin>>NW;

    int num_matrices ; 
    cout << "Enter number of matrices to be multiplied : " ; 
    cin >> num_matrices ; 

    // storing start time to calculate total time taken 
    time_t s = time(0) ;

    // ftok : key generation 
    key_t val = ftok("/dev/random",'c');
    
    // Shared Memory ID creation and checking allocation 
    size_t shared_memory_size = sizeof(SM)+sizeof(FiniteQueue);
    int smi ;  
    if( ( smi = shmget(IPC_PRIVATE, shared_memory_size, 0666 | IPC_CREAT) ) < 0 ){
        perror("ERROR : ") ; 
        return -1 ; 
    }

    // shared memory creation
    // NULL : indicates shared memory segment to be attached to the first pointer allocated by 
    // the system
    // 0 : read/write permission

    SM* Smem = (SM*)shmat(smi, NULL, 0); ; 

    FiniteQueue* Smem_queue =(FiniteQueue*)( Smem + 1);
    Smem->Q = Smem_queue;
    Smem->job_created = 0 ; 
    Smem->Mno = num_matrices ; 
    Smem->Q->front = 0;
    Smem->Q->size = 0;
   // Smem->job_done = 0;

    // process IDs for processes , workers 
    pid_t producers[NP], workers[NW] ;  

    int k = 0 ;

    while(k < NP){

        // temporary PID for operations
        pid_t tpid = fork() ; 



        // child process; producer creation
        if(!tpid){
             sleep((rand()%100)/100.0);
            if(Smem->job_created < Smem->Mno)
            {
             createProducer(Smem,k) ;
            
            }
             shmdt(Smem);
            exit(0);
        }

        // parent process
        else if(tpid > 0){
            //printf("Heelooo\n");
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

    int w = 0 ;
    while(w < NW){
        cout<<"WORKERS"<<endl;
        // temporary PID for operations
        pid_t tpid = fork() ; 

        // child process; producer creation
        if(!tpid){
            cout<<Smem->Q->size<<endl;
            while(1)
            {
                if((Smem->Q->size)<2)
                {

                    sleep(1);
                    continue;
                }
                cout<<"HELLOOO"<<endl;
            createWorker(Smem, NP, NW, w) ;
            cout<<"BYEEEEE"<<endl;
            }
            shmdt(Smem);
            exit(0);
        }

        // parent process
        else if(tpid > 0){
            // storing process id in array
            workers[w++] = tpid ;
        }

        // pid < 0 : fork error 
        else{
            w++ ;
            perror("ERROR : fork error \n") ;
            return -1 ; 
        }
    }

    // killing using process ids stored during worker and producer creation 
    for(;;){
        // condition termination
        if((Smem->job_created >= num_matrices) && (Smem->Q->size == 1)){
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
    shmctl(smi, IPC_RMID, 0) ;

    return 0 ; 
}
