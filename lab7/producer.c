#include<stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <semaphore.h>
#include <fcntl.h>

#define NUMBER 20 
#define BUFSIZE 10 
sem_t   *empty, *full, *mutex;
int main(){
	
	int  i;
 	int  buf_in = 0;
 	if((mutex = sem_open("mutex",O_CREAT, 0644, 1)) == SEM_FAILED)
    	{
        	perror("sem_open() error!\n");
        	return -1;
   	 }
    if((empty = sem_open("empty",O_CREAT, 0644,10)) == SEM_FAILED)
    {
        perror("sem_open() error!\n");
        return -1;
    }
    if((full = sem_open("full",O_CREAT, 0644,0)) == SEM_FAILED)
    {
        perror("sem_open() error!\n");
        return -1;
    }
	
	int shmid = shmget(7,BUFSIZE*sizeof(int), IPC_CREAT);
	printf("shmid: %d\n", shmid);
	int *p=(int *)shmat(shmid, NULL, 0);
	for( i = 0 ; i < NUMBER; i++)
    {
	
        sem_wait(empty);
        sem_wait(mutex);
	p[buf_in] = i;
	printf("produce: %d\n", i);
        buf_in = ( buf_in + 1)% BUFSIZE;
        sem_post(mutex);
        sem_post(full);
    }
	sem_unlink("full");
    	sem_unlink("empty");
    	sem_unlink("mutex");
	return 0;
}
