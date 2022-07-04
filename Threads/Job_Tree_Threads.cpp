#include <bits/stdc++.h>
#include <pthread.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
using namespace std;

int MAX_JOBS = 30000;
#define MAX_DEPENDENT_JOBS 100
#define MAX_THRDS 50

struct job {
	int job_id;
	int time_of_completion;
	int status;  // -1 -> empty, 0 -> just created , 1-> ongoing 
	int num_dependent;
	int dependent_jobs[MAX_DEPENDENT_JOBS];
	pthread_mutex_t lock;
};

struct Tree {
	job* jobs;
	pthread_mutex_t lock;
	int num_jobs;
	int* thread;
};

Tree *tree;
int P;

pthread_t producer_threads[MAX_THRDS], consumer_threads[MAX_THRDS];
pthread_mutexattr_t lock_attr;



job create_job() {
	job j;
	j.job_id = 1 + rand()%100000000;
	j.time_of_completion = rand()%251;
	j.status = 0;
	for(int i = 0; i < MAX_DEPENDENT_JOBS; i++) {
		j.dependent_jobs[i] = -1;
	}
	j.num_dependent = 0;
	pthread_mutex_init(&(j.lock), &lock_attr);
	return j;
}


void *producer_runner(void *param) {
	int tid = *(int*)param;
	int running_time = 10 + rand()%11;
	running_time *= 1000;
	int cur_time = 0;
	while(cur_time < running_time) {
		pthread_mutex_lock(&tree->lock);
		vector<pair<int,int>> indices;
		for(int i = 0; i < MAX_JOBS; i++) {
			if((tree->jobs)[i].status == 0 || (tree->jobs)[i].status == -1) {
				indices.push_back({(tree->jobs)[i].status, i});
			}
		}
		
		if((int)indices.size() == 0) {
			pthread_mutex_unlock(&tree->lock);
			break;
		}
		random_shuffle(indices.begin(), indices.end());
		pair<int,int> to_put = indices[rand()%(indices.size())];
		job j = create_job();


		for(int i = 0; i < MAX_JOBS; i++) {
			if((tree->jobs)[i].status == -1) {
				(tree->jobs)[i] = j;
				break;
			}
		}
		tree->num_jobs += 1;
		cout << "New job created: " << j.job_id << endl;

		if(to_put.second == 0) {
			pthread_mutex_lock(&((tree->jobs)[to_put.second].lock));
		}

		pthread_mutex_unlock(&tree->lock);

		if(to_put.first == 0) {
			for(int i = 0; i < MAX_DEPENDENT_JOBS; i++) {
				if((tree->jobs)[to_put.second].dependent_jobs[i] != -1) {
					continue;
				}
				(tree->jobs)[to_put.second].dependent_jobs[i] = j.job_id;
				(tree->jobs)[to_put.second].num_dependent += 1;
				break;
			}
			pthread_mutex_unlock(&((tree->jobs)[to_put.second].lock));
		}
		int sleep_time = rand()%301 + 200;
		if(usleep(sleep_time*1000) == -1) {
			printf("Error in sleep\n");
			exit(1);
		}
		cur_time += sleep_time;
	}
	(tree->thread)[tid] = 1;
	free(param);
	pthread_exit(0);
}

void *consumer_runner(void *param) {
	while(1) {
		pthread_mutex_lock(&tree->lock);

		if((tree->num_jobs) <= 0) {
			int flag = 0;
			for(int i=0; i<P; i++) {
				if((tree->thread)[i] == 0) {
					flag = 1;
					break;
				}
			}
			if(flag == 0) {
				pthread_mutex_unlock(&tree->lock);
				break;
			}
			else {
				pthread_mutex_unlock(&tree->lock);
				continue;
			}
		}
		int job_to_execute = -1;
		for(int i = 0; i < MAX_JOBS; i++) {
			if((tree->jobs)[i].status == 0 && (tree->jobs)[i].num_dependent == 0) {
				// execute this job
				job_to_execute = i;
				break;
			}
		}
		if(job_to_execute == -1) {
			int flag = 0;
			for(int i=0; i<P; i++) {
				if((tree->thread)[i] == 0) {
					flag = 1;
					break;
				}
			}
			if(flag == 0) {
				pthread_mutex_unlock(&tree->lock);
				break;
			}
			else {
				pthread_mutex_unlock(&tree->lock);
				continue;
			}
		}
		(tree->jobs)[job_to_execute].status = 1;
		cout << "Start of job: " << (tree->jobs)[job_to_execute].job_id << endl;
		pthread_mutex_unlock(&tree->lock);
		int time_to_sleep = (tree->jobs)[job_to_execute].time_of_completion;
		
		if(usleep(time_to_sleep*1000) == -1) {
			printf("Error in sleep\n");
			exit(1);
		}
		
		pthread_mutex_lock(&tree->lock);
		cout << "End of job: " << (tree->jobs)[job_to_execute].job_id << endl;
		(tree->jobs)[job_to_execute].status = -1;
		for(int i = 0; i < MAX_JOBS; i++) {
			if(i == job_to_execute) {
				continue;
			}
			if((tree->jobs)[i].status == 0) {
				for(int j = 0; j < MAX_DEPENDENT_JOBS; j++) {
					if((tree->jobs)[i].dependent_jobs[j] == (tree->jobs)[job_to_execute].job_id) {
						(tree->jobs)[i].dependent_jobs[j] = -1;
						(tree->jobs)[i].num_dependent -= 1;
						break;
					}
				}
			}
		}
		(tree->num_jobs) -= 1;
		pthread_mutex_unlock(&tree->lock);
	}
	pthread_exit(0);
}


int main() {

	cout << "Enter the number of producer threads: ";
	cin >> P;
	int C;
	cout << "Enter the number of consumer threads: ";
	cin >> C;

	MAX_JOBS = 500 + P*100 + 1000;

	key_t key = 235;
	int shmid = shmget(key, sizeof(Tree), IPC_CREAT|0666);
	if(shmid < 0) {
		printf("Error in creating shared memory\n");
		exit(1);
	}
	tree = (Tree *) shmat(shmid, NULL, 0);

	key_t key1 = 230;
	int shmid1 = shmget(key1, sizeof(job) * MAX_JOBS, IPC_CREAT|0666);
	if(shmid1 < 0) {
		cout << "Error in creating shared memory\n";
		exit(1);
	}
	(tree->jobs) = (job*) shmat(shmid1, NULL, 0);

	key_t key2;
	int shmid2;
	if(P != 0) {
		key2 = 240;
		shmid2 = shmget(key2, sizeof(int) * P, IPC_CREAT|0666);
		if(shmid2 < 0) {
			cout << "Error in creating shared memory\n";
			exit(1);
		}
		(tree->thread) = (int*) shmat(shmid2, NULL, 0);
	}

	pthread_mutexattr_init(&lock_attr);
	pthread_mutexattr_setpshared(&lock_attr, PTHREAD_PROCESS_SHARED);

	for(int i = 0; i < MAX_JOBS; i++) {
		(tree->jobs)[i].status = -1;
	}

	for(int i=0; i < P; i++) {
		(tree->thread)[i] = 0;
	}

	int num_jobs = 300 + rand()%201;
	tree->num_jobs = num_jobs;
	for(int i = 0; i < num_jobs; i++) {
		(tree->jobs)[i] = create_job();
	}
	
	pthread_mutex_init(&tree->lock, &lock_attr);
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	for(int i = 0; i < P; i++) {
		int* temp = (int*)malloc(sizeof(int));
		*temp = i;
		pthread_create(&producer_threads[i], &attr, producer_runner, temp);
	}
	
	pid_t pid;
	pid = fork();
	if(pid == 0) {
		
		for(int i = 0; i < C; i++) {
			pthread_create(&consumer_threads[i], &attr, consumer_runner, NULL);
		}
		for(int i = 0; i < C; i++) {
			pthread_join(consumer_threads[i], NULL);
		}
		if(P != 0) {
			shmdt(tree->thread);
		}
		shmdt(tree->jobs);
		shmdt(tree);
		exit(0);
	}
	else {
		for(int i = 0; i < P; i++) {
			pthread_join(producer_threads[i], NULL);
		}
		wait(NULL);
	}
	if(P != 0) {
		shmdt(tree->thread);
		shmctl(shmid2, IPC_RMID, 0);
	}
	shmdt(tree->jobs);
	shmctl(shmid1, IPC_RMID, 0);
	shmdt(tree);
	shmctl(shmid, IPC_RMID, 0);
	
	return 0;
}