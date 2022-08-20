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
	
	sem_unlink("full");
    	sem_unlink("empty");
    	sem_unlink("mutex");
	return 0;
}
