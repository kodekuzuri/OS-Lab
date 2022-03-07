#include <pthread.h>
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>

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
	int ready_children_count;
	int array_index;
	//pthread_mutex_t lock;    // mutex lock
	int status;
	// Other data
	void create_node(int,int,int,int);
    void remove_child() ;
	void add_child(int);
};

struct shared_memory
{
	Node base_tree[MAX_JOBS];
	int jobs_created;
	int add_job(int, int);
} *A;

void Node::remove_child(){
    this->ready_children_count = this->ready_children_count - 1 ; 
}

void Node::create_node(int job_id,int run_time,int parent,int array_index)
{
	this->job_id = job_id;
    this->completion_time = run_time;
    this->parent_node = parent;
    this->array_index = array_index;
    this->status = 0;
    this->ready_children_count = 0;
    this->child_index = 0;
}

int shared_memory::add_job(int parent_node, int print_flag)
{
    int flag ;

	int index = this->jobs_created;
	(this->base_tree)[index].create_node(rand()%MAX_JOBS+1,rand()%MAX_COMPLETION_TIME+1,parent_node,index);
	(this->jobs_created)++;


    //PRINT
        if(print_flag){
            cout << endl ;
            cout << "ADDED NEW JOB SUCCESSFULLY\n" ; 

            cout << "JOB ID = " << (this->base_tree)[index].job_id << endl ; 
            cout << "JOB COMPLETION TIME = " << (this->base_tree)[index].completion_time << " ms" << endl ; 
            cout << "INDEX OF PARENT =  " << parent_node << endl ; 

            cout << endl ; 
        }
    //PRINT END
    
	return index;
}


void Node::add_child(int print_flag)
{	
	(this->dependent_jobs)[this->child_index] = A->add_job(this->array_index, print_flag);
	this->child_index++;
	this->ready_children_count++;

}




void *producer_job(void *argp)
{
	// Set seed
	srand(pthread_self());

	int producer_thread_time = 	 10 + (rand()%11);
	auto start_time =  chrono::high_resolution_clock::now();

	while(chrono::duration_cast<chrono::seconds>(chrono::high_resolution_clock::now() - start_time).count() < producer_thread_time)
	{
		int node_id = rand()%A->jobs_created;
		int flag = 0;
		// Node is free (ready to use)
		if((A->base_tree)[node_id].status==0)
		{
			(A->base_tree)[node_id].status = 1;
			flag = 1;
		}

		if(flag==1)
		{

			(A->base_tree)[node_id].add_child(1);

			(A->base_tree)[node_id].status = READY;

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

    if(!(A->base_tree)[u].status){


        if((A->base_tree)[u].ready_children_count == READY){
            if(!flag) flag = 1 ; 
            (A->base_tree)[u].status = RUNNING_C ; 
        }

        else ; 
    }

    if(flag == 1){

        //PRINT
            cout << endl ;
            cout << " NEW JOB STARTED SUCCESSFULLY\n" ; 

            cout << "JOB ID = " << (A->base_tree)[u].job_id << endl ; 
            cout << "JOB COMPLETION TIME = " << (A->base_tree)[u].completion_time  << " ms" << endl ; 
            cout << "INDEX OF PARENT =  " << (A->base_tree)[u].parent_node << endl ; 

            cout << endl ; 
        //PRINT END

		int sleepdur = (A->base_tree)[u].completion_time ; 
        sleepdur *= 1000 ;
        
        usleep( sleepdur ) ;   

        //PRINT
            cout << endl ;
            cout << "JOB DONE SUCCESSFULLY" << endl ; 

            cout << "JOB ID = " << (A->base_tree)[u].job_id << endl ; 
            cout << "JOB COMPLETION TIME = " << (A->base_tree)[u].completion_time  << " ms" << endl ; 
            cout << "INDEX OF PARENT =  " << (A->base_tree)[u].parent_node << endl ; 

            cout << endl ; 
        //PRINT END 

        (A->base_tree)[u].status = DONE ; 
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

        val = (A->base_tree)[root].status ; 
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
    // this->jobs_update.init();
    // this->output_log.init();

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

        // do nothing 
        else ; 
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
