#define __LIBRARY__
#include <unistd.h>
#include <linux/sem.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/sched.h>

_syscall2(sem_t *,sem_open,const char *,name,unsigned int,value)
_syscall1(int,sem_wait,sem_t *,sem)
_syscall1(int,sem_post,sem_t *,sem)
_syscall1(int,sem_unlink,const char *,name)

const char *FILENAME = "/usr/root/buffer_file"; 
const int NR_CONSUMERS = 5;         
const int NR_ITEMS = 15;                      
const int BUFFER_SIZE = 10;                    
sem_t *metux, *full, *empty;                   
unsigned int item_pro, item_used;               
int fi, fo;                                  


int main(int argc, char *argv[])
{
    char *filename;
    int pid;
    int i;

    filename = argc > 1 ? argv[1] : FILENAME;

    fi = open(filename, O_CREAT| O_TRUNC| O_WRONLY, 0222);   
    fo = open(filename, O_TRUNC| O_RDONLY, 0444);           

    metux = sem_open("METUX", 1);   
    full = sem_open("FULL", 0);        
    empty = sem_open("EMPTY", BUFFER_SIZE);   

    item_pro = 0;

    if ((pid = fork()))  
    {
        
        fflush(stdout);

        while (item_pro <= NR_ITEMS)  
        {
            sem_wait(empty);
            sem_wait(metux);
            if(!(item_pro % BUFFER_SIZE))
                lseek(fi, 0, 0);

            write(fi, (char *) &item_pro, sizeof(item_pro));
            
            fflush(stdout);
            item_pro++;

            sem_post(full); 
            sem_post(metux);
        }
    }
    else 
    {
        i = NR_CONSUMERS;
        while(i--)
        {
            if(!(pid=fork()))
            {
                pid = getpid();
                
                fflush(stdout);

                while(1)
                {
                    sem_wait(full);
                    sem_wait(metux);
                    if(!read(fo, (char *)&item_used, sizeof(item_used)))
                    {
                        lseek(fo, 0, 0);
                        read(fo, (char *)&item_used, sizeof(item_used));
                    }

                    printf("pid %d:\tconsumer %d consumes item %d\n", pid, NR_CONSUMERS-i+1, item_used);
                    fflush(stdout);

                    sem_post(empty);   
                    sem_post(metux);
                    if(item_used == NR_ITEMS)
                        goto OK;
                }
            }
        }
    }
OK:
    close(fi);
    close(fo);
    return 0;
}

