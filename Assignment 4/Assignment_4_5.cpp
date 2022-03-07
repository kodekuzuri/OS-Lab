// NAME: ARYAN MEHTA PRANAV RAJPUT
// ROLL NUMBER: 19CS30006 19CS30036
// GROUP 5

#include <iostream>
#include <pthread.h>
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>

// FLAGS USED FOR STATUS OF JOB
#define READY 0
#define RUNNING_P 1
#define RUNNING_C 2
#define DONE -1

#define MAX_JOBS 1000
#define MAX_COMPLETION_TIME 250

using namespace std;


struct Node
{
	int job_id;
	int completion_time;
	int dependent_jobs[MAX_JOBS]; // dependent jobs of that node (children)
	int parent_node;
	int child_index; 
	int ready_children_count; // children count of node which are not run yet
	int array_index;
	pthread_mutex_t node_lock ;    // mutex lock for node
	int status;
	void create_node(int,int,int,int);
    void remove_child() ;
	void add_child(int);

};

struct shared_memory
{
	Node base_tree[MAX_JOBS];
	int jobs_created;
    pthread_mutex_t print_lock, job_lock;  // print lock used for output mutex and job lock when job created has a change 
	int add_job(int, int); // Add new job (node to tree)
} *A;
// A is the master process


void Node::create_node(int job_id,int run_time,int parent,int array_index)
{
    // NODE MUTEX LOCK  INITIALISATION
    pthread_mutexattr_t attribute ;
    pthread_mutexattr_init(&attribute);
    pthread_mutexattr_setpshared(&attribute, PTHREAD_PROCESS_SHARED);
    pthread_mutexattr_settype(&attribute, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&(this->node_lock), &attribute);

	this->job_id = job_id;
    this->completion_time = run_time;
    this->parent_node = parent;
    this->array_index = array_index;
    this->status = 0;
    this->ready_children_count = 0;
    this->child_index = 0;
}

void Node::remove_child(){

	pthread_mutex_lock(&(this->node_lock)) ; 
    this->ready_children_count = this->ready_children_count - 1 ; 
    pthread_mutex_unlock(&(this->node_lock)) ; 
}

void Node::add_child(int print_flag)
{
	pthread_mutex_lock(&(this->node_lock)) ; 	
	(this->dependent_jobs)[this->child_index] = A->add_job(this->array_index, print_flag);
	this->child_index++;
	this->ready_children_count++;
	pthread_mutex_unlock(&(this->node_lock)) ; 
}


int shared_memory::add_job(int parent_node, int print_flag)
{

    pthread_mutex_lock(&(this->job_lock)) ; 

	int index = this->jobs_created;
	(this->base_tree)[index].create_node(rand()%MAX_JOBS+1,rand()%MAX_COMPLETION_TIME+1,parent_node,index);
	(this->jobs_created)++;


    //print flag is a variable used so that only new jobs added are printed not the inital ones.
        if(print_flag){
            pthread_mutex_lock(&(this->print_lock)) ; 

            cout << endl ;
            cout << "ADDED NEW JOB SUCCESSFULLY\n" ; 

            cout << "JOB ID = " << (this->base_tree)[index].job_id << endl ; 
            cout << "JOB COMPLETION TIME = " << (this->base_tree)[index].completion_time << " ms" << endl ; 
            cout << "INDEX OF PARENT =  " << parent_node << endl ; 

            cout << endl ; 

            pthread_mutex_unlock(&(this->print_lock)) ; 
        }
    //PRINT END

    pthread_mutex_unlock(&(this->job_lock)) ; 
    
	return index;
}






void *producer_job(void *argp)
{
	// Set seed
	srand(pthread_self());

	int producer_thread_time = 	 10 + (rand()%11);
	auto start_time =  chrono::high_resolution_clock::now();

	while(chrono::duration_cast<chrono::seconds>(chrono::high_resolution_clock::now() - start_time).count() < producer_thread_time)
	{
        int flag = 0 ;

        pthread_mutex_lock(&(A->job_lock)) ; 

		int node_id = rand()%A->jobs_created;

        pthread_mutex_unlock(&(A->job_lock)) ; 

		
		// Node is free (ready to use)

       pthread_mutex_lock(&((A->base_tree)[node_id].node_lock)) ;  

		if((A->base_tree)[node_id].status==0)
		{
			(A->base_tree)[node_id].status = 1;
			flag = 1;
		}

       pthread_mutex_unlock(&((A->base_tree)[node_id].node_lock)) ;  


		if(flag==1)
		{

			(A->base_tree)[node_id].add_child(1);

       		pthread_mutex_lock(&((A->base_tree)[node_id].node_lock)) ;  
			(A->base_tree)[node_id].status = READY;
       		pthread_mutex_unlock(&((A->base_tree)[node_id].node_lock)) ;  

			int sleepdur = 200 + rand()%301 ; 
        	usleep(sleepdur*1000) ;   
		}

	}
	pthread_exit(0);
}


void tree_traversal(int u)
{
    int flag ; 

    flag = 0 ; 

    pthread_mutex_lock(&((A->base_tree)[u].node_lock)) ;  


    if (((A->base_tree)[u].status == DONE) || ((A->base_tree)[u].status == RUNNING_C)) {
        pthread_mutex_unlock(&((A->base_tree)[u].node_lock)) ;  

        return ; 
    }


    if(!(A->base_tree)[u].status){

        if((A->base_tree)[u].ready_children_count == READY){
            if(!flag) flag = 1 ; 
            (A->base_tree)[u].status = RUNNING_C ; 
        }

        pthread_mutex_unlock(&((A->base_tree)[u].node_lock)) ;  

    }

    // node unlock

    if(flag == 1){

        pthread_mutex_lock(&(A->print_lock)) ; 

        //PRINT
            cout << endl ;
            cout << " NEW JOB STARTED SUCCESSFULLY\n" ; 

            cout << "JOB ID = " << (A->base_tree)[u].job_id << endl ; 
            cout << "JOB COMPLETION TIME = " << (A->base_tree)[u].completion_time  << " ms" << endl ; 
            cout << "INDEX OF PARENT =  " << (A->base_tree)[u].parent_node << endl ; 

            cout << endl ; 
        //PRINT END

        pthread_mutex_unlock(&(A->print_lock)) ; 

		int sleepdur = (A->base_tree)[u].completion_time ; 
        sleepdur *= 1000 ;
        
        usleep( sleepdur ) ;  

        pthread_mutex_lock(&(A->print_lock)) ;  

        //PRINT
            cout << endl ;
            cout << "JOB DONE SUCCESSFULLY" << endl ; 

            cout << "JOB ID = " << (A->base_tree)[u].job_id << endl ; 
            cout << "JOB COMPLETION TIME = " << (A->base_tree)[u].completion_time  << " ms" << endl ; 
            cout << "INDEX OF PARENT =  " << (A->base_tree)[u].parent_node << endl ; 

            cout << endl ; 
        //PRINT END 

        pthread_mutex_unlock(&(A->print_lock)) ; 

		 pthread_mutex_lock(&((A->base_tree)[u].node_lock)) ;  
 
        (A->base_tree)[u].status = DONE ; 
       	pthread_mutex_unlock(&((A->base_tree)[u].node_lock)) ;  

        (A->base_tree)[(A->base_tree)[u].parent_node].remove_child() ; 
    }

    else {
        int ctr = 0 ;

        while( ctr  < (A->base_tree)[u].child_index ){
            tree_traversal((A->base_tree)[u].dependent_jobs[ctr++]) ;  
        }
    }
}

void *consumer_job(void *argc)
{
    int val, root = 0 ; 

    val = 1 ; 

    while(! ((val == DONE) || (val == RUNNING_C)) ){

        // call to tree traversal from root node
        tree_traversal(root) ; 

       pthread_mutex_lock(&((A->base_tree)[root].node_lock)) ;  
        val = (A->base_tree)[root].status ; 
       pthread_mutex_unlock(&((A->base_tree)[root].node_lock)) ;  

    }

    pthread_exit(0) ; 

  //  return ;
}


int main(int argc, char const *argv[])
{

	int P;
	cout<<"Enter number of producer threads"<<endl;
	cin>>P;

	int y;
	cout<<"Enter number of consumer threads"<<endl;
	cin>>y;
	
	int shmid;
	shmid = shmget(IPC_PRIVATE,sizeof(shared_memory),IPC_CREAT | 0666);

	void* shared_mem = shmat(shmid, NULL, 0) ;

	A = (shared_memory *)shared_mem;
	A->jobs_created = 0;

    // SETTING THREAD ATTRIBUTES
    pthread_mutexattr_t attribute ;
    pthread_mutexattr_init(&attribute);
    pthread_mutexattr_setpshared(&attribute, PTHREAD_PROCESS_SHARED);
    pthread_mutexattr_settype(&attribute, PTHREAD_MUTEX_RECURSIVE);

    pthread_mutex_init(&(A->print_lock), &attribute);
    pthread_mutex_init(&(A->job_lock), &attribute);  
   


	A->add_job(-1, 0); // Add 1st node

	int num_initial_jobs = 300 + (rand()%201);

	// Add initial jobs
	for (int i = 0; i < num_initial_jobs; ++i)
	{
		/* code */
		int random_node_id = rand()%A->jobs_created;
		(A->base_tree)[random_node_id].add_child(0);

	}

	pthread_t producer_threads[P];
	for (int i = 0; i < P; ++i)
	{
		/* code */
		// thread creation
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_create(producer_threads + i, &attr, producer_job, NULL);
	}

    int k = 0 ; 

    while(k < P){
        pthread_join(producer_threads[k], NULL);
        k++ ;
    }

    cout<<"\nProducer jobs done"<<endl;

    int consumer_pid = fork();

    if(consumer_pid == -1){
        perror("Consumer process fork error: ");
        exit(0);
    }

    else {
        if(consumer_pid == 0) 
            {

                pthread_t consumer_threads[y];

                int i = 0 ;  
                while(i < y)
                {
                    pthread_attr_t attr;
                    pthread_attr_init(&attr);
                    pthread_create(consumer_threads + i, &attr, consumer_job, NULL);
                    i++ ; 
                }

                i = 0 ; 
                while(i < y){
                    pthread_join(consumer_threads[i], NULL);
                    i++ ; 
                }

                exit(0);

            } 

       
    }

    // parent waiting for child termination
    for(;wait(NULL) > 0;) ;

    cout << endl ; 
    cout << "Consumer Process has been completed" <<endl ;

    // detaching shared memory 
    shmdt(shared_mem);
    shmctl(shmid, IPC_RMID, NULL);

	return 0;
}
