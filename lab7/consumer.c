/*consumer.c*/
#include<stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <semaphore.h>
#include <fcntl.h>
#define NUMBER 20 
#define BUFSIZE 10 

sem_t  *empty, *full, *mutex;
int main()
{
    int  i,shmid,data;
    int *p;
    int  buf_out = 0;
    
    if((mutex = sem_open("mutex",O_CREAT, 0644,1)) == SEM_FAILED)
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
	shmid = shmget(7, BUFSIZE*sizeof(int), IPC_CREAT);
	if(shmid == -1)
    {
        return -1;
    }
    p = (int *)shmat(shmid,NULL,0);
	printf("consumer start.\n");
        fflush(stdout);
    for( i = 0; i < NUMBER; i++ )
    {
	
        sem_wait(full);
        sem_wait(mutex);
	
        data = p[buf_out];
        buf_out = (buf_out + 1) % BUFSIZE;
	printf("%d:  %d\n",getpid(),data);
        fflush(stdout);
        sem_post(mutex);
        sem_post(empty);
        
        
    }
    printf("consumer end.\n");
    fflush(stdout);
    sem_unlink("full");
    sem_unlink("empty");
    sem_unlink("mutex");

    return 0;
}

