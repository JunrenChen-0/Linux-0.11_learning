#define __LIBRARY__
#include <unistd.h>
#include <linux/sem.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/sched.h>

_syscall2(sem_t *, sem_open, const char *, name, unsigned int, value)
_syscall1(int, sem_wait, sem_t *, sem)
_syscall1(int, sem_post, sem_t *, sem)
_syscall1(int, sem_unlink, const char *, name)


#define M 15
#define N 5
#define BUFSIZE 10

int buf_out=0;
int real_out=0;
int buf_in = 0;
unsigned int item_used;
sem_t *Empty;
sem_t *Mutex;
sem_t *Full;


void Producer(int fd){	
	int i=0;
	for(;i<M;++i){
		sem_wait(Empty);
		sem_wait(Mutex);
		lseek(fd, buf_in*sizeof(int), SEEK_SET);
		write(fd, (char *)&i, sizeof(int));
		buf_in = (buf_in+1)%BUFSIZE;
		
		sem_post(Mutex);
		sem_post(Full);
		
	}
	printf("producer end.\n");
	fflush(stdout);
}

void Counsumer(int fd){
	int i=0;
	
	while(1){
		sem_wait(Full);
		sem_wait(Mutex);

		lseek(fd, (BUFSIZE+1)*sizeof(int), SEEK_SET);
		read(fd, (char *)&real_out, sizeof(int));
		if(real_out == M-1){
			sem_post(Mutex);
			sem_post(Full);
			break;
		}
		lseek(fd, BUFSIZE*sizeof(int), SEEK_SET);
		read(fd, (char *)&buf_out, sizeof(int));
		
		lseek(fd, buf_out*sizeof(int), SEEK_SET);
		read(fd, (char *)&item_used, sizeof(int));

		buf_out = (buf_out+1)%BUFSIZE;
		lseek(fd, BUFSIZE*sizeof(int), SEEK_SET);
		write(fd, (char *)&buf_out,sizeof(int));
		printf("%d: %d\n", getpid(), item_used);
		fflush(stdout);
		sem_post(Mutex);
		sem_post(Empty);
		if(item_used == M-1){
			real_out=M-1;
			lseek(fd, (BUFSIZE+1)*sizeof(int), SEEK_SET);
			write(fd,(char *)&real_out,sizeof(int));
			sem_post(Full);
			break;
		}
	}
}

int main(){
	int fd=-1;
	int pid;
	int i=0;
	if((fd=open("buffer.txt", O_CREAT|O_TRUNC|O_RDWR,0666))==-1){
		printf("error\n");
		return 0;
	}
	Empty = sem_open("Empty", 10);
	Mutex = sem_open("Mutex", 1);
	Full = sem_open("Full", 0);
	lseek(fd, BUFSIZE*sizeof(int), SEEK_SET);
	write(fd,(char *)&buf_out,sizeof(int));
	lseek(fd, (BUFSIZE+1)*sizeof(int), SEEK_SET);
	write(fd,(char *)&real_out,sizeof(int));
	if((pid = fork())==0){
		Producer(fd);
		exit(0);
	}
	for(;i<N;++i){
		if((pid=fork())==0){
			Counsumer(fd);
			printf("%d finished\n",getpid());
			exit(0);
		}
	}
	
	for(i=0;i<N+1;++i){
		wait(NULL);
	}
	printf("father finished\n");
	sem_unlink("Empty");
	sem_unlink("Mutex");
	sem_unlink("Full");
	close(fd);
	return 0;
}
