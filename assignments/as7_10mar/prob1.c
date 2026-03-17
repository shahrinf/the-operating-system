

/*creates a file (if it does not exist)  fname of appropriate size (4096 + bsize*bno Bytes) in your folder. The function initializes the first 4096 Byes putting proper values for n, s, ubn, fbn, and ffb. Since at the beginning all the bno blocks are free, ffb can be made 0 (the 1st block) and at the end of each block, next block number can be written. If for some reason this functions fails, it returns -1. Otherwise it returns 0.
*/

int init_File_dd(const char *fname, int bsize, int bno){

fname=(char*)malloc(4096+bsize*bno);
if(fname==null){
return -1;
}

//initialising the 1st block
int n=2048,s=4096,ubn=0,fbn=4096;

memcpy(fname,&n,sizeof(int));
memcpy(fname+sizeof(int),&s,sizeof(int));
memcpy(fname+2*sizeof(int),&ubn,sizeof(int));
memcpy(fname+3*sizeof(int),&ubn,sizeof(int));

//initialising all the free block ptrs , present at the end of each block
for(int i=0;i<n;i++){
memcpy(fname+i*4096-sizeof(int),&i,sizeof(int));
}

//last block will have -1 in place of ffb ,since there are no more free blocks after it, so will be the case for any block that doesnt have any more free blocks after it
i=-1;
memcpy(fname+n*4096-sizeof(int),&i,sizeof(int));

return 0;
}

/*reads the 1st 4096 Bytes (containing n, s, ubn, fbn, ffb) from the file fname  and finds and returns the block-number of the 1st free block (starting from the 0th bloc) in that file. This block should be removed from the free-block list. ubn and fbn too  are modified appropriately.  On failure this function returns -1. Please  note that all the modifications done by this function have to be written back to the file fname.
*/

int get_freeblock(const char *fname){
int flag=0;
int ffb;
memcpy(&ffb,fname+4096-sizeof(int),sizeof(int));

//in case there are no more free blocks
if(ffb==-1)
return -1;


}

/*reads the 1st 4096 Bytes (containing n, s, ubn, fbn, ffb) from the file fname  and frees the block bno, that is,  adds this block to the free-block list. This function  fills up the  block bno with 0's.  ubn and fbn too  are modified appropriately.  On success  this function returns 1. It returns 0 otherwise.  Please note that all the modifications done by this function have to be written back to the file fname.
*/

//int free_block(const char *fname, int bno){}

/*checks the integrity of the file fname with respect to n, s, ubn, fbn, ffb and the contents of the blocks. In case of any inconsistency (say, ubn+fbn ≠  n, free-block list does not contain fbn number of blocks, etc. this function returns 1. It returns 0 otherwise.
*/

int check_fs(const char *fname){
int n,s,ubn,fbn,ffb;
memcpy(&n,fname,sizeof(int));
memcpy(&s,fname+sizeof(int),sizeof(int));
memcpy(&ubn,fname+2*sizeof(int),sizeof(int));
memcpy(&ubn,fname+3*sizeof(int),sizeof(int));

if((ubn+fbn)!=n)
return 1;
}

int main(){


}
