#include <stdio.h>
#include <stdlib.h> /* needed for exit() */
#include <fcntl.h> /* Needed for open() */
#include <errno.h> /* Needed for perror */
#include <unistd.h> /* Needed for read() */
#include <errno.h> /* Needed for perror */
#include <sys/stat.h> /* Needed for umask() */
#include <string.h>


#define MIN_ROLL 10000


typedef struct student{
	int roll; /* storing the roll number of the student */
	char *fname; /* storing the first name of the student */
}student;

/* prints all of the student record */
int print_all(int datafd,int indexfd);


/* prints the student record whose index is present 
 * at 'off_of_off' offset of index file 
 */
int print_student(int datafd,int indexfd, off_t off_of_off);


int input_record(int datafd,int indexfd);

int compaction(char* datafile, char* idxfile);

/* structure followed by a record:
 *rollno
 *fname length
 *fname
 */
int read_intobuffer(int datafd, int idx, char* buff);

int delete(int datafd,int indexfd, int rollno);

/* param rollno : rollno of the record to be searched
 * return value : returns offset of the index if record found, else -1
 */ 
off_t search(int datafd,int indexfd, int rollno);

/*helper function for sort*/
off_t index_of_min(int datafd, int indexfd, off_t start, off_t end);


void sort(int datafd, int indexfd);
/*Sorts index file
 *according to rollno
 *in the corresponding data record
 *using selection sort
 */

int main(int argc, char *argv[]) {
	int datafd, indexfd, success=0, rollno, flag;
	int n = 0;
	off_t off_of_off;
	if (argc != 3) {
		fprintf(stderr, "Usage %s <data file> <index file>\n", argv[0]);
		exit(2);
	}
	umask(0); /*to restrict the default access*/

	while (1) {
		int option;
		printf("0 (Initialize)\n");
		printf("1 (Add a record)\n");
		printf("2 (Delete a record)\n");
		printf("3 (Search for a record)\n");
		printf("4 (Modify a record)\n");
		printf("5 (Compaction)\n");
		printf("6 (Print a student record)\n");
		printf("7 (Sort by rollno)\n");
		printf("8 (print all the records)\n");
		printf("Give your option: ");
		scanf("%d%*[^\n]", &option);

		switch (option) {
			case 0:
				printf("Initializing\n");
				// int open(const char *pathname, int flags, ...  /* mode_t mode */ );
				datafd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC,0777);
				if (datafd == -1 ) {
					perror("Open() failed for student.data");
					exit(2);
				}
				//intializing student.index
				indexfd = open(argv[2], O_RDWR | O_CREAT | O_TRUNC,0777);
				if (indexfd == -1 ) {
					perror("Open() failed for student.index");
					exit(2);
				}

				//initial no_of_records(n) = 0
				flag=write(datafd,&n,sizeof(int));
				if (flag == -1) {
					perror(" .data write() failed");
					exit(1);
				}

				flag=write(indexfd,&n,sizeof(int));
				if (flag == -1) {
					perror(" .index write() failed");
					exit(1);
				}
				close(datafd);
				close(indexfd);				
				break;

			case 1: //input a record

				datafd = open(argv[1], O_RDWR);
				if (datafd == -1) {
					perror("open() failed");
					exit(1);
				}
				indexfd = open(argv[2], O_RDWR);
				if (indexfd == -1) {
					perror("open() failed");
					exit(1);
				}

				flag=input_record(datafd,indexfd);

				printf("Record input successful\n");

				close(datafd);
				close(indexfd);
				break;

			case 2:
				// deleting a record
				printf("enter the rollno to be deleted\n");
				scanf("%d%*[^\n]",&rollno);

				datafd = open(argv[1], O_RDWR);
				if (datafd == -1) {
					perror("open() failed");
					exit(1);
				}
				indexfd = open(argv[2], O_RDWR);
				if (indexfd == -1) {
					perror("open() failed");
					exit(1);
				}

				flag= delete(datafd,indexfd,rollno); //0->success
				if(flag)
					printf("Record deletion unsuccessful as no such student exists:(\n");

				else
					printf("Record deletion successful:)\n");

				close(datafd);
				close(indexfd);
				break;

			case 3:
				// searching a record
				printf("enter the rollno to be searched\n");
				scanf("%d%*[^\n]",&rollno);

				datafd = open(argv[1], O_RDONLY);
				if (datafd == -1) {
					perror("open() failed");
					exit(1);
				}
				indexfd = open(argv[2], O_RDONLY);
				if (indexfd == -1) {
					perror("open() failed");
					exit(1);
				}


				off_of_off=search(datafd,indexfd,rollno);
				if(off_of_off==-1)
					printf("student not found\n");

				else{
					printf("student found! at index:%lld\n",off_of_off);

					print_student(datafd,indexfd,off_of_off);
				}
				close(datafd);
				close(indexfd);

				break;
			case 4:
				//modify
				printf("enter the rollno of the student to be modified\n");
				scanf("%d%*[^\n]",&rollno);

				datafd = open(argv[1], O_RDWR | O_APPEND);  //lseek will work for read ops only
				if (datafd == -1) {
					perror("open() failed");
					exit(1);
				}
				indexfd = open(argv[2], O_RDWR);
				if (indexfd == -1) {
					perror("open() failed");
					exit(1);
				}

				//delete prev record, add new record
				success= delete(datafd,indexfd,rollno); //0->success
				if(success){//when search fails
					perror("No such record exists:(\n");

				}
				else{
					//input new record()
					success=input_record(datafd, indexfd);
					printf("Record modification successful\n");
				}
				close(datafd);
				close(indexfd);

				break;

			case 5:
				//compaction
				//files are opened from inside the function
				printf("performing compaction\n");

				success=compaction( argv[1], argv[2]);
				printf("Compaction successful\n");

				break;

			case 6:
				//print a student with a given rollno
				printf("enter the rollno of the student\n");
				scanf("%d%*[^\n]",&rollno); 

				datafd = open(argv[1], O_RDONLY);
				if (datafd == -1) {
					perror("open() failed");
					exit(1);
				}
				indexfd = open(argv[2], O_RDONLY);
				if (indexfd == -1) {
					perror("open() failed");
					exit(1);
				}

				//searching the offset of the index of the record with 'rollno' as rollno
				off_of_off=search(datafd,indexfd,rollno);
				if(off_of_off==-1)
					printf("No such student exists:(\n");
				else
					print_student(datafd,indexfd,off_of_off);
				break;

			case 7: printf("sorting the .index file on the basis of rollnos\n");

				//compaction();
				compaction(argv[1],argv[2]);

				datafd = open(argv[1], O_RDONLY);
				if (datafd == -1) {
					perror("open() failed");
					exit(1);
				}
				indexfd = open(argv[2], O_RDWR);
				if (indexfd == -1) {
					perror("open() failed");
					exit(1);
				}

				//sorting				
				sort(datafd,indexfd);
                                printf("done!\n");
				close(datafd);
				close(indexfd);
				break;

			case 8: //print all student record as ordered in index file

				datafd = open(argv[1], O_RDONLY);
				if (datafd == -1) {
					perror("open() failed");
					exit(1);
				}
				indexfd = open(argv[2], O_RDONLY);
				if (indexfd == -1) {
					perror("open() failed");
					exit(1);
				}
				print_all(datafd,indexfd);
 close(datafd);
                                close(indexfd);
				break;
			default:
				printf("enter valid option\n");
		}
	}
}


int print_all(int datafd,int indexfd){
	int flag;

	flag=lseek(indexfd,0,SEEK_SET);
	if (flag == -1) {
		perror(" .index lseek() failed");
		exit(1);
	}
	int n;
	flag=read(indexfd,&n,sizeof(int));
	if (flag == -1 || flag!=sizeof(int)) {
		perror(" .index read() failed");
		exit(1);
	} 
	off_t off_of_off=sizeof(int); //initial offset 

	while(n>0){
		flag=print_student(datafd,indexfd,off_of_off);
		off_of_off+=sizeof(off_t);
		//student exists then flag=0, decrease count
		if(!flag)
			n--;
	}
	return 0;
}



int print_student(int datafd,int indexfd, off_t off_of_off){
	off_t flag;

	flag=lseek(indexfd,off_of_off,SEEK_SET);
	if (flag == -1) {
		perror(" .index lseek() failed");
		exit(1);
	}
	off_t idx;
	flag=read(indexfd,&idx,sizeof(off_t));
	if (flag == -1||flag!=sizeof(off_t)) {
		perror(" .index read() failed");
		exit(1);
	} 

	if(idx==-1){
		return 1;
	}
	flag=lseek(datafd,idx,SEEK_SET);
	if (flag == -1) {
		perror(" .data lseek() failed");
		exit(1);
	} 
	int rollno;

	flag=read(datafd,&rollno,sizeof(int));
	if (flag == -1||flag!=sizeof(int)) {
		perror(" .data read() failed");
		exit(1);
	}
	int fname_len;
	flag=read(datafd,&fname_len,sizeof(int));
	if (flag == -1||flag!=sizeof(int)) {
		perror(" .data read() failed");
		exit(1);
	}
	char fname[fname_len];
	flag=read(datafd,fname,fname_len);
	if (flag == -1) {
		perror(" .data read() failed");
		exit(1);
	}
	printf("student info:\nrollno : %d\nfirst name : %s\n",rollno,fname);
	return 0;
}

//insert record into the file
int input_record(int datafd,int indexfd){
	off_t flag;
	flag=lseek(indexfd,0,SEEK_SET);
	if (flag == -1) {
		perror(" .index lseek() failed");
		exit(1);
	}
	int n;
	flag=read(indexfd,&n,sizeof(int));
	if (flag == -1||flag!=sizeof(int)) {
		perror(" .index read() failed");
		exit(1);
	}
	/* incrementing no_of_records in .data and .index files */
	n+=1;
	//repositioning the fildes to the beginning of file
	/* off_t lseek(int fd, off_t offset, int ref); */
	flag=lseek(datafd,0,SEEK_SET);
	if (flag == -1) {
		perror(" .index lseek() failed");
		exit(1);
	}
	flag=write(datafd,&n,sizeof(int));
	if (flag == -1||flag!=sizeof(int)) {
		perror(" .data write() failed");
		exit(1);
	}
	flag=lseek(indexfd,0,SEEK_SET);
	if (flag == -1) {
		perror(" .index lseek() failed");
		exit(1);
	}
	flag=write(indexfd,&n,sizeof(int));
	if (flag == -1||flag!=sizeof(int)) {
		perror(" .index write() failed");
		exit(1);
	}
	//saving offset in .index
	off_t pos=lseek(datafd,0,SEEK_END); // finding start offset of next record, also setting datafd to end of records
	if (pos == -1) {
		perror(" .index lseek() failed");
		exit(1);
	}	
	flag=lseek(indexfd,0,SEEK_END); // setting indexfd to end of indices
	if (flag == -1) {
		perror(" .index lseek() failed");
		exit(1);
	}
	flag=write(indexfd,&pos,sizeof(off_t)); // setting index of the latest record
	if (flag == -1) {
		perror(" .index write() failed");
		exit(1);
	}
	/* writing a record into .data file */

	int b_wr; /* no. of bytes written */
	student s;
	printf("Enter student details:\n");
	printf("Rollno.:\n");
	scanf("%d%*[^\n]", &s.roll);
	// ssize_t write(int fd, const void* buf, size_t bytes);
	// datafd above set to end of records
	b_wr=write(datafd,&s.roll,sizeof(int));
	if (b_wr == -1) {
		perror(" .data write() failed");
		exit(1);
	}
	printf("First name.:\n");
	s.fname=(char*)malloc(50*sizeof(char));
	scanf("%s", s.fname);
	int len=strlen(s.fname);
	len++;
	b_wr=write(datafd,&len,sizeof(int));
	if (b_wr == -1) {
		perror(" .data write() failed");
		exit(1);
	}
	b_wr=write(datafd,s.fname,(strlen(s.fname)+1)); //names end with null char
	if (b_wr == -1) {
		perror(" .data write() failed");
		exit(1);
	}
	free(s.fname);

	return 0;
}

int read_intobuffer(int datafd, int idx, char* buff){ //buff starts from new record position
	off_t flag;
	int len=0; //data currently written to buffer since this fn call
	flag=lseek(datafd,idx,SEEK_SET);
	if (flag == -1) {
		perror(" .index lseek() failed");
		exit(1);
	}
	//read rollno
	flag=read(datafd,buff,sizeof(int));
	if (flag == -1) {
		perror(" .data read() failed");
		exit(1);
	}
	len=sizeof(int);
	//read fname_len
	int fname_len;
	flag=read(datafd,&fname_len,sizeof(int));
	if (flag == -1) {
		perror(" .data read() failed");
		exit(1);
	}
	memcpy(buff+len,&fname_len,sizeof(int));
	len+=sizeof(int);
	//read fname
	flag=read(datafd,buff+len,fname_len);
	if (flag == -1) {
		perror(" .data read() failed");
		exit(1);
	}
	len+=fname_len;

	return len;
}


int compaction(char* datafile, char* idxfile){
	off_t flag;
	int indexfd=open(idxfile,O_RDWR,0777);
	if (indexfd == -1) {
		perror("open() failed");
		exit(1);
	}
	int datafd=open(datafile,O_RDWR,0777);
	if (datafd == -1) {
		perror("open() failed");
		exit(1);
	}
	off_t end=lseek(indexfd,0,SEEK_END);
	if (end == -1) {
		perror(" .index lseek() failed");
		exit(1);
	}
	int n;
	flag=lseek(indexfd,0,SEEK_SET);
	if (flag == -1) {
		perror(" .index lseek() failed");
		exit(1);
	}

	off_t off_of_off=read(indexfd,&n,sizeof(int)); //start offset of idx
	if (off_of_off == -1) {
		perror(" .index read() failed");
		exit(1);
	}

	//buffer to read all valid records into
	off_t data_end=lseek(datafd,0,SEEK_END);
	if (data_end == -1) {
		perror(" .data lseek() failed");
		exit(1);
	}
	char* data_buff=(char*)malloc(data_end);
	//check for malloc error

	//buffer for updated indexes
	off_t idx_buff[n];
	//read a valid record from .data one by one into a buffer and then write the entire buffer back to the truncated file

	int idx;
	off_t len=0; //stores length of data written into data_buffer at any time, new index
	int i=0; //counter for no of records written at any time

	while(off_of_off<end){

		//updating the index
		idx_buff[i]=len+sizeof(int);

		flag=read(indexfd,&idx,sizeof(off_t));
		if (flag == -1) {
			perror(" .index read() failed");
			exit(1);
		}
		off_of_off+=sizeof(off_t);	
		//if the record is deleted, move on to next
		if(idx==-1)
			continue;

		len+=read_intobuffer(datafd,idx,data_buff+len); //returns len of data read
		i++;

	}
	close(datafd);
	close(indexfd);

	printf("\n");
	//truncating the files
	indexfd=open(idxfile,O_WRONLY|O_TRUNC,0777);
	if (indexfd == -1) {
		perror("open() failed");
		exit(1);
	}
	datafd=open(datafile,O_WRONLY|O_TRUNC,0777);
	if (datafd == -1) {
		perror("open() failed");
		exit(1);
	}
	//finally writing entire buffer back to file
	flag=write(indexfd,&n,sizeof(int));
	if (flag== -1) {
		perror(" .index write() failed");
		exit(1);
	}
	flag=write(indexfd,idx_buff,sizeof(off_t)*n);
	if (flag== -1) {
		perror(" .index write() failed");
		exit(1);
	}
	flag=write(datafd,&n,sizeof(int));
	if (flag== -1) {
		perror(" .data write() failed");
		exit(1);
	}
	flag=write(datafd,data_buff,len);
	if (flag == -1) {
		perror(" .data write() failed");
		exit(1);
	}

	free(data_buff);

	close(datafd);
	close(indexfd);

	return 0;
}

//returns 0 if success, 1 otherwise
int delete(int datafd,int indexfd, int rollno){
	int flag;

	//searching the offset of the index of the record with 'rollno' as rollno
	off_t off_of_off=search(datafd,indexfd,rollno);

	if(off_of_off==-1)
		return 1;

	//reading the count of records
	flag=lseek(indexfd,0,SEEK_SET);
	if (flag == -1) {
		perror(" .index lseek() failed");
		exit(1);
	}	
	int n;
	flag=read(indexfd,&n,sizeof(int));
	if (flag == -1) {
		perror(" .index read() failed");
		exit(1);
	}

	if(off_of_off!=-1){
		flag=lseek(indexfd,off_of_off,SEEK_SET);
		if (flag == -1) {
			perror(" .index lseek() failed");
			exit(1);
		}		
		off_t del=-1;
		flag=write(indexfd,&del,sizeof(off_t));
		if (flag == -1) {
			perror(" .index write() failed");
			exit(1);
		}
		//decreasing the count of records in index as well as data file
		flag=lseek(indexfd,0,SEEK_SET);
		if (flag == -1) {
			perror(" .index lseek() failed");
			exit(1);
		}	
		n--;
		flag=write(indexfd,&n,sizeof(int));
		if (flag == -1) {
			perror(" .index read() failed");
			exit(1);
		}
		flag=lseek(datafd,0,SEEK_SET);
		if (flag == -1) {
			perror(" .data lseek() failed");
			exit(1);
		}

		flag=write(datafd,&n,sizeof(int));
		if (flag == -1) {
			perror(" .data write() failed");
			exit(1);
		}
		return 0;
	}
	perror("search unsuccessful\n");
	return 1;
}

/* search */
off_t search(int datafd,int indexfd, int rollno){
	// searching each index
	int cnt,flag;
	flag=lseek(indexfd,0,SEEK_SET);
	if (flag == -1) {
		perror(" .index lseek() failed");
		exit(1);
	}
	flag=read(indexfd,&cnt,sizeof(int)); //setting cnt=n as well as moving cursor on indexfd to the first index
	if (flag == -1 || flag!=sizeof(int)) {
		perror(" .data read() failed");
		exit(1);
	}	
	while(cnt>0){
		off_t off_of_off=lseek(indexfd,0,SEEK_CUR); //current index's offset, potential return value
		if (off_of_off == -1) {
			perror(" .index lseek() failed");
			exit(1);
		}		
		off_t index;
		flag=read(indexfd,&index,sizeof(off_t));
		if (flag == -1) {
			perror(" .index read() failed");
			exit(1);
		}
		//non existent
		if(index==-1)
			continue;
		cnt--;		
		int rno;
		//reading rollno
		flag=lseek(datafd,index,SEEK_SET);
		if (flag == -1) {
			perror(" .data lseek() failed");
			exit(1);
		}
		flag=read(datafd,&rno,sizeof(int));
		if (flag == -1) {
			perror(" .data read() failed");
			exit(1);
		}

		if(rno!=rollno)
			continue;


		return off_of_off;
	}
	return -1;
}



/* param start: offset of the index to begin searching from
 * param end: offset where the index file ends
 * return value: returns the offset of the offset of the min record
 */
off_t index_of_min(int datafd, int indexfd, off_t start, off_t end){

	int flag;

	flag = lseek(indexfd, start, SEEK_SET);
	if (flag == -1) {
		perror(" .index lseek() failed");
		exit(1);
	}

	int min_roll=MIN_ROLL; /* initial min roll*/
	int offset_toreturn=start; /*stores the return value*/
	int roll_read;
	off_t record_offset;
	off_t indexOffset=start;/*loops through indices starting from start*/

	while( indexOffset < end ){

		/* reading offset of the record*/

		flag=read(indexfd,&record_offset,sizeof(off_t));
		if (flag == -1 || flag != sizeof(off_t)) {
			perror(" .index read() failed");
			exit(1);
		}

		/*extracting the rollno of the record whose offset is record_offset*/

		flag=lseek(datafd,record_offset,SEEK_SET);
		if (flag == -1) {
			perror(" .data lseek() failed");
			exit(1);
		}

		flag=read(datafd,&roll_read,sizeof(int));
		if (flag == -1 || flag!=sizeof(int) ) {
			perror(" .data read() failed");
			exit(1);
		}
		/* roll_found, now comparing with current min*/
		if(roll_read<min_roll){
			min_roll=roll_read;
			offset_toreturn=indexOffset;
		}

		indexOffset+=8;
	}

	return offset_toreturn;
}


/* selection sort */
void sort(int datafd, int indexfd){

	int flag;
	int n;

	/*reading n:no of records*/
	flag = read(indexfd,&n,sizeof(int));
	if (flag == -1 || flag != sizeof(int)) {
		perror("read() failed");
		exit(1);
	}

	off_t end=4+n*8; /*offset of end of index file*/
	off_t off_of_off = 4; /* stores current offset of indices in index file*/

	/* both curr_idx and idx_toswap are offsets of indices*/
	off_t curr_idx; //points to current offset
	off_t idx_toswap; //points to offset that is to be swapped with curr_idx

	/*curr_val and val_toswap are indices of records kept at positions curr_idx and idx_swap of the .index file resp*/
	off_t curr_val;
	off_t val_toswap;

	for(int i=0;i<n;i++){

		curr_idx=4+(8*i);

		idx_toswap=index_of_min(datafd,indexfd,curr_idx,end);


		/*index already at correct position*/
		if(idx_toswap==curr_idx)
		{
			continue;
		}
		/*reading the indices*/

		flag=lseek(indexfd,curr_idx,SEEK_SET);
		if (flag == -1) {
			perror(" .index lseek() failed");
			exit(1);
		}
		flag = read(indexfd,&curr_val,sizeof(off_t));
		if (flag == -1 || flag != sizeof(off_t)) {
			perror(" .index read() failed");
			exit(1);
		}

		flag=lseek(indexfd,idx_toswap,SEEK_SET);
		if (flag == -1) {
			perror(" .index lseek() failed");
			exit(1);
		}

		flag = read(indexfd,&val_toswap,sizeof(off_t));
		if (flag == -1 || flag != sizeof(off_t)) {
			perror(" .index read() failed");
			exit(1);
		}



		/* now writing/actually swapping the offsets */

		flag=lseek(indexfd,curr_idx,SEEK_SET);
		if (flag == -1) {
			perror(" .index lseek() failed");
			exit(1);
		}
		flag = write(indexfd,&val_toswap,sizeof(off_t));
		if (flag == -1 || flag != sizeof(off_t)) {
			perror(" .index write() failed");
			exit(1);
		}


		flag=lseek(indexfd,idx_toswap,SEEK_SET);
		if (flag == -1) {
			perror(" .index lseek() failed");
			exit(1);
		}

		flag = write(indexfd,&curr_val,sizeof(off_t));
		if (flag == -1 || flag != sizeof(off_t)) {
			perror(" .index write() failed");
			exit(1);
		}
	}
}


