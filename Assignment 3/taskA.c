#include<stdio.h>
#include <stdlib.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/wait.h>
#include<sys/types.h>


typedef struct process_data
{
    double **A;
    double **B;
    double **C;
    int veclen,row,col;

}ProcessData;

void mult(ProcessData *current_process)
{
    int i = current_process->row;
    int j = current_process->col;
    current_process->C[i][j] = 0;

    int veclen = current_process->veclen;
    for(int k = 0;k<veclen;++k)
    {
        current_process->C[i][j] += current_process->A[i][k]*current_process->B[k][j];
    }
}
int main() {
    
    int r1,r2,c1,c2;
    int shared_mem_id;
    printf("Enter  rows and columns for matrix 1: \n");
    scanf("%d%d",&r1,&c1);
    printf("Enter rows and columns for matrix 2: \n");
    scanf("%d%d",&r2,&c2);

    if(c1!=r2)
    {
        printf("Number of columns of matrix 1 should be equal to number of rows of matrix 2\n");
        return 0;
    }

    double *Atemp[r1];
    double *Btemp[r2];
    double *Ctemp[r1];

    size_t s = (r1*c1+r2*c2+r1*c2)*sizeof(double);
    shared_mem_id = shmget(IPC_PRIVATE,s,IPC_CREAT | 0644);
    if(shared_mem_id<0)
    {
        printf("Error in shmget\n");
        return -1;
    }
    double* shared_memory = (double*)shmat(shared_mem_id,0,0);

    for (int i = 0; i < r1; ++i)
    {
        /* code */
        Atemp[i] = shared_memory + i*c1;
    }

    // double** A = (double**)Atemp;

    // shmidB = shmget(11,r2*c2*sizeof(double),IPC_CREAT);
    // if(shmidB<0)
    // {
    //     printf("Error in shmget\n");
    //     return -1;
    // }
    //  double* memB = (double*)shmat(shmidB,0,0);

    for (int i = 0; i < r2; ++i)
    {
        /* code */
        Btemp[i] = shared_memory + r1*c1 + i*c2;
    }

    // double** B = (double**)Btemp;

    // shmidC = shmget(12,r1*c2*sizeof(double),IPC_CREAT);
    // if(shmidC<0)
    // {
    //     printf("Error in shmget\n");
    //     return -1;
    // }

    //  double* memC = (double*)shmat(shmidC,0,0);

    for (int i = 0; i < r1; ++i)
    {
        /* code */
        Ctemp[i] = shared_memory+ r2*c2+r1*c1 + i*c2;
    }

    double** A = (double**)Atemp;
    double** B = (double**)Btemp;

    double** C = (double**)Ctemp;

    printf("Enter matrix 1: \n");
    for(int i=0;i<r1;++i)
        for(int j=0;j<c1;++j)
            scanf("%lf",&A[i][j]);
    printf("Enter matrix 2: \n");   
    for(int i=0;i<r2;++i)
        for(int j=0;j<c2;++j)
            scanf("%lf",&B[i][j]);   

    
    
   
    for (int i = 0; i < r1; ++i)
      {
          /* code */
        for(int j = 0; j < c2;++j)
        {
            pid_t pid = fork(); 
            if(pid==0)
            {
                ProcessData *current = (ProcessData*)malloc(sizeof(ProcessData)) ;
                current->row = i;
                current->col = j;
                current->veclen = c1;
                current->A = A;
                current->B = B;
                current->C = C;
                mult(current);
                shmdt(shared_memory);
                exit(0);
            }
           
        }
      }

   for (int i = 0; i < r1*c2; ++i)
   {
       /* code */
        wait(NULL);
   }
   printf("Matrix C = A*B is given below: \n");

    for (int i = 0; i < r1; ++i)
    {
        /* code */
        for (int j = 0; j < c2; ++j)
        {
            /* code */
            printf("%lf  ",C[i][j] );
        }
        printf("\n");
    }
   shmdt(shared_memory); shmctl(shared_mem_id, IPC_RMID, 0);

    return 0;
}
