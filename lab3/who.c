#include <asm/segment.h>
#include <errno.h>
char nameaddr[23];
int len=0;
void copy(char *temp){
	int i=0;
	for(;i<len;++i){
		nameaddr[i]=temp[i];
	}
}

int sys_iam(const char *name){
	char tempName[23];
	char *temp = tempName;	
	char c;
	int count=0;
	while( (c = get_fs_byte(name)) != '\0' ){
		count++;
		if(count==24){
			return -EINVAL;
		}
		*temp=c;
		name++;
		temp++;
	}
	len=count;
	copy(tempName);
	return count;
}
int sys_whoami(char*name, unsigned int size){
	int i=0;
	if(size<len){
		errno=EINVAL;
		return -EINVAL;
	}
	for(;i<len;++i){
		put_fs_byte(nameaddr[i], name);
		name++;
	}
	return len;
}
