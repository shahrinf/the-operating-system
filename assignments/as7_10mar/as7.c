#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

char* buf[4096];


/*creates a file (if it does not exist)  fname of appropriate size (4096 + bsize*bno Bytes) in your folder. The function initializes the first 4096 Byes putting proper values for n, s, ubn, fbn, and ffb. Since at the beginning all the bno blocks are free, ffb can be made 0 (the 1st block) and at the end of each block, next block number can be written. If for some reason this functions fails, it returns -1. Otherwise it returns 0.
 */

int init_File_dd(const char *fname, int bsize, int bno){

	int fd=open(fname,O_CREAT|O_RDWR,0777);

	int n=2048,s=4096,ubn=0,fbn=2048,ffb=0;
	memcpy(buf,&n,4);
	memcpy(buf+4,&s,4);
	memcpy(buf+8,&ubn,4);
	memcpy(buf+12,&fbn,4);
	memcpy(buf+16,&ffb,4);

	for(int i=1;i<n;i++){
		lseek(fd,i*4096-4,SEEK_SET);
		write(fd,&i,4);
	}
	lseek(fd,n*4096-4,SEEK_SET);
	int j=-1;
	write(fd,&j,4);
	close(fd);
	return 0;
}

/*reads the 1st 4096 Bytes (containing n, s, ubn, fbn, ffb) from the file fname  and finds and returns the block-number of the 1st free block (starting from the 0th bloc) in that file. This block should be removed from the free-block list. ubn and fbn too  are modified appropriately.  On failure this function returns -1. Please  note that all the modifications done by this function have to be written back to the file fname.
 */

int get_freeblock(const char *fname){

	int fd=open(fname,O_RDWR,0777);
	read(fd,buf,4096);
	int ubn,fbn,ffb,next_fb;
	memcpy(&ffb,buf+16,4);

	if(ffb==-1)
		return -1;
	memcpy(&ubn,buf+8,4);
	memcpy(&fbn,buf+12,4);
	ubn++;
	fbn--;
	memcpy(buf+8,&ubn,4);
	memcpy(buf+12,&fbn,4);
	lseek(fd,(ffb+1)*4096+4092,SEEK_SET);
	read(fd,&next_fb,4);
	memcpy(buf+16,&next_fb,4);
	//also change next_fb for each used block that was before the original first free block
	for(int i=0;i<ffb;i++){
		lseek(fd,4096+i*4096+4092,SEEK_SET);
		write(fd,&next_fb,4);
	}

	//updating ubn and fbn
	lseek(fd,0,SEEK_SET);
	write(fd,buf,4096);
	return ffb;
	close(fd);
	return ffb;

}

/*reads the 1st 4096 Bytes (containing n, s, ubn, fbn, ffb) from the file fname  and frees the block bno, that is,  adds this block to the free-block list. This function  fills up the  block bno with 0's.  ubn and fbn too  are modified appropriately.  On success  this function returns 1. It returns 0 otherwise.  Please note that all the modifications done by this function have to be written back to the file fname.
 */

int free_block(const char *fname, int bno){

	int BNO=bno;
	int fd=open(fname,O_RDWR,0777);
	read(fd,buf,4096);
	int ubn,fbn,ffb,next_fb;

	memcpy(&ubn,buf+8,4);
	memcpy(&fbn,buf+12,4);
	ubn--;
	fbn++;
	memcpy(buf+8,&ubn,4);
	memcpy(buf+12,&fbn,4);

	//for each block before bno that has bno as the next free block, change their next free block to next-next free block
	//first find next-next free block->at the end of bno
	int nnfb;
	lseek(fd,4096+bno*4096+4092,SEEK_SET);
	read(fd,&nnfb,4);
	int nfb=bno;
	while(1){
		--bno;
		lseek(fd,4096+bno*4096+4092,SEEK_SET);
		read(fd,&nfb,4);
		if(nfb!=BNO)
			break;
		lseek(fd,4096+bno*4096+4092,SEEK_SET);
		write(fd,&nnfb,4);
	}
	return 1;
}


/*checks the integrity of the file fname with respect to n, s, ubn, fbn, ffb and the contents of the blocks. In case of any inconsistency (say, ubn+fbn ≠  n, free-block list does not contain fbn number of blocks, etc. this function returns 1. It returns 0 otherwise.
 */

int check_fs(const char *fname){

}

int main(){
}
