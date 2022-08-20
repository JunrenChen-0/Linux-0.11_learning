#include "stdio.h"
#define __LIBRARY__
#include "unistd.h"
_syscall2(int, whoami,char*,name,unsigned int,size);
int main(int argc,char** argv){
	int count=0, i=0;
	char name[23];
	count = whoami(name, 23);
	for(;i<count;++i){
		printf("%c", name[i]);
	}
	printf("\n");
	return 0;
}
