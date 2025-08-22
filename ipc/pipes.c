#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h> //for open()
#include <sys/types.h> //for pid_t
#include <sys/stat.h>   // for file permission macros
#include <errno.h>      // for errno (optional with perror)

#define BUFFER_SIZE 1024

int main(int argc, char* argv[]){

if(argc<2)
{
//fprintf prints, as well as log in the stderr file
//perror logs the error, with the argument as the name plus the technical description of the error provided by the system

fprintf(stderr,"usage: .\\pipe file1 file2");
exit(1);
}

char buffer[BUFFER_SIZE];

//creating two files, for the two ends of the pipe
int fd[2];

//creating the pipe
pipe(fd);

pid_t pid=fork();

if(pid>0){
close(fd[0]);//closing read end==0

int src=open(argv[1],O_RDONLY,0644); //read=4, write=2, read+write=6

ssize_t bytes;
while((bytes=read(src,buffer,sizeof(buffer)))>0){// reading from the given source file

//writing to the write end of pipe
write(fd[1],buffer,bytes);//bytes store number of bytes read from the file described by the file descriptor

}

close(src);
close(fd[0]);
}


//child inherits open files from parent
if(pid==0){
close(fd[1]);//closing write end==1

int dest=open(argv[2],O_CREAT|O_WRONLY,0644);

ssize_t bytes;
//reading from the read end of the pipe
while((bytes=read(fd[0],buffer,sizeof(buffer)))>0){ // read will wait until data is available in fd[0], else if EOF is reached, it will return 0, and in case of error, returns -1.


//writing to the destination file
write(dest,buffer,bytes);
}

close(dest);
close(fd[1]);

}

return 0;
}


