#include "stdio.h"
#define __LIBRARY__
#include "unistd.h"
#include "errno.h"
_syscall1(int, iam, const char*, name);
int main(int argc,char** argv){
	int count=-1, i=1;
	char str[1000];
	char *sp = str;
	if(argc<2){
		printf("miss arg");
		return 0;
	}
	for(;i<argc;++i){
		char *p = argv[i];
		while(*p!='\0'){
			*sp=*p;
			sp++;
			p++;
		}
		*sp=' ';
		sp++;
	}
	sp--;
	*sp='\0';
	sp = str;
	count = iam(str);
	return 0;
}
