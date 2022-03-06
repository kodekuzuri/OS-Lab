#include <pthread.h>
#include <iostream>
#define MAX_JOBS 500

using namespace std;


struct Node
{
	int job_id;
	double completion_time;
	int dependent_jobs[MAX_JOBS];
	pthread_mutex_t lock;    // mutex lock
	int status;
	// Other data

};


int main(int argc, char const *argv[])
{
	
	return 0;
}