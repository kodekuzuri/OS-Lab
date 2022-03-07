#include <pthread.h>
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>
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
	void add_child();
};

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

void Node::add_child()
{	
	(this->dependent_jobs)[this->child_index] = A->add_job(this->array_index);
	this->child_index++;
	this->ready_children_count++;
}
struct shared_memory
{
	Node base_tree[MAX_JOBS];
	int jobs_created;
	int add_job(int);
} *A;


int shared_memory::add_job(int parent_node)
{
	int index = this->jobs_created;
	(this->base_tree)[index].create(rand()%MAX_JOBS+1,rand()%MAX_COMPLETION_TIME+1,parent_node,index);
	(this->jobs_created)++;

	return index;
}
void *producer_job(void *argp)
{
	// Set seed
	srand(pthread_self());

	int producer_thread_time = 	 10 + (rand()%11);
	auto start_time =  chrono::high_resolution_clock::now();

	while(chrono::duration_cast<chrono::seconds>(chrono::high_resolution_clock::now() - start).count() < producer_thread_time)
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
			(A->base_tree)[node_id].add_new_child();

			(A->tree)[node_id].status = 0;

			int sleepdur = rand()%501 ; 
        	usleep(sleepdur*1000) ;   
		}

	}
	pthread_exit(0);
}

void *consumer_job(void *argc)
{

}
int main(int argc, char const *argv[])
{

	int P;
	cout<<"Enter number of producer threads"<<endl;
	cin>>P;

	int y;
	cout<<"Enter number of consymer threads"<<endl;
	cin>>y;
	
	int shmid;
	shmid = shmget(IPC_PRIVATE,sizeof(shared_memory),IPC_CREAT | 0666);

	shared_mem = shmat(shmid,NULL,0);
	A = (shared_memory *)shared_mem;
	A->jobs_created = 0;
    // this->jobs_update.init();
    // this->output_log.init();

	A->add_job(-1); // Add 1st node

	int num_initial_jobs = 300 + (rand()%201);

	// Add initial jobs
	for (int i = 0; i < num_initial_jobs; ++i)
	{
		/* code */
		int random_node_id = rand()%A->jobs_created;
		(A->base_tree)[random_node_id].add_child();

	}

	pthread_t producer_threads[P];
	for (int i = 0; i < P; ++i)
	{
		/* code */
		 pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_create(producer_threads + i, &attr, producer_job, NULL);
	}

	 for(int i = 0; i < prod_thread_cnt; i++)
        pthread_join(producer_threads[i], NULL);
    cout<<"\nProducer jobs done"<<endl;

    int consumer_pid = fork();
    if(consumer_pid == 0) 
    {

        pthread_t consumer_threads[y];
        for(int i = 0; i < y; i++) 
        {
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            pthread_create(consumer_threads + i, &attr, consumer_job, NULL);
        }

        for(int i = 0; i < y; i++)
            pthread_join(consumer_threads[i], NULL);

        exit(0);

    } else if(consumer_pid == -1) 
    {
        perror("Consumer process fork error: ");
        exit(0);
    }

     while(wait(NULL) > 0) ;
    cout<<"\nConsumer process done."<<endl;

     shmdt(shared_mem);
    shmctl(shmid, IPC_RMID, NULL);

	return 0;
}
