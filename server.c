#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <time.h>
#include<sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>

int spot;
int globalbadFiles = 0;
int globaldirNum = 0;
int globalregFiles = 0;
int globalspecialFiles = 0;
off_t globalbytes = 0;
int globaltextnum = 0;
off_t globaltextBytes = 0;
pthread_t tids[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

 struct rusage usage;
 double intime = 0;
 double nexttime = 0;
 struct timeval time1;





// create semaphores.
sem_t sem1;


// functions to set the semaphores
void seminit(){
  sem_init(&sem1,0,1);
}


void incrementValues(char* filename){
  char* temp = filename;
  struct stat sbuff;
  //printf("------>>>>>>>>this thread opened %s\n",filename);
  if(stat(temp,&sbuff) == 0){
    //got files with no error
    //check if the file is a dir
    if  (S_ISDIR(sbuff.st_mode) > 0){
      globaldirNum++;
    }
    //check if is a normal file
    else if(S_ISREG(sbuff.st_mode) != 0){
      int fin,cnt;
      int flag = 0;
      char textBuff[1024];
      //open the file
      if((fin = open(temp,O_RDONLY)) > 0){
        // read the file into textBuff
          //printf("%d --> :\n",fin);
        while((cnt = read(fin,textBuff,1024)) > 0 && flag != 1){
          // check character by character to see if they are space of characters
          //printf("%s\n",textBuff);
          for(int i = 0; i < cnt;i++){
            //char a = textBuff[i];
            int n = isspace(textBuff[i]);
            int p = isprint(textBuff[i]);
            if(n == 0 && p  == 0){
              flag =1;
              break;
            }
          }

        }

        if (fin > 0){
          close(fin);
        }

        if(flag == 0){
          globaltextnum++;
          globaltextBytes = sbuff.st_size + globaltextBytes;
        }
      }
      globalregFiles++;
      globalbytes =	sbuff.st_size + globalbytes;
    }
    //if is not one of the above si a special file
    else{
      globalspecialFiles++;
    }

  }
  else{
    globalbadFiles++;
  }

}



void* threadFunction (void *filename){
  char* file = (char*)filename;

//  printf("---------%s\n", file);

  sem_wait(&sem1);
  incrementValues(file);
  sem_post(&sem1);
}


void main (int argc, char* argv[]){
 
  gettimeofday(&time1,NULL);
  intime = ((double) time1.tv_sec *(double)1000) + ((double) time1.tv_usec / (double)1000);
  // max number of threads
  int tnum = 0;
  // number of running threads at a given time
  int actualtnum = 0;
  // char array to store file names
  char buff[1024] = " ";
  // check if we have two arguments
  if(argc > 2){
    // check that the second argument is the word "thread"
    if(strcmp(argv[1], "thread") == 0){
      //convert string to integer for thread num

      tnum = atoi(argv[2]);
      // check for max value of thread less than 15
      if(tnum > 15){
        printf("The max number of trheads is 15\n");
        exit(0);
      }
      // sempahores intitialize
      seminit();
      int newfile = 0;
      int createdt = 0;
      //while to read filename by filename


    char tempvar[1024];
    while(fgets(buff,1024,stdin) != NULL){
            char*  filename = malloc(sizeof(char*[1024]));
          if(buff[0] != EOF ){
          sscanf(buff,"%s",filename);

          if(newfile < tnum){
           pthread_create(&tids[newfile % tnum], NULL,threadFunction,(void *)filename);
          }else{
              pthread_join(tids[newfile % tnum],NULL);
              pthread_create(&tids[newfile % tnum], NULL,threadFunction,(void *)filename);
            }
    //        printf(" this trhead is %d\n",newfile % tnum  );
           newfile++;


      }
      else{
      break;
    }
}
  }
}
  else{
    char filename[1024] = " ";
    while(fgets(buff,1024,stdin) != NULL){
      if(buff[0] != '\n' && buff[0] != EOF){
        sscanf(buff,"%s",filename);
        incrementValues(filename);

      }

    }
  }
      if(argc > 2){
  //      printf("loop nu\n");
	
        for(int i = 0;i < tnum;i++){
//	printf(" the t id is %ld\n  ",tids[i]);
	  if(tids[i] != 0){
         pthread_join(tids[i],NULL);
        }
	}

      }
	gettimeofday(&time1,NULL);
        nexttime = ((double) time1.tv_sec *(double)1000) + ((double) time1.tv_usec / (double)1000);
        getrusage( RUSAGE_SELF,&usage);
	printf("User CPU time %f\n", ((double) usage.ru_utime.tv_sec *(double)1000) + ((double)usage.ru_utime.tv_usec /(double) 1000));          /* wait for the child to finish */
        printf("System  CPU time %f\n", ((double) usage.ru_stime.tv_sec *(double)1000) + ((double)usage.ru_stime.tv_usec /(double) 1000));
        printf("The total time that process took was %f\n\n\n",nexttime-intime);
	

      printf(" Bad files: %d \n",globalbadFiles);
      printf(" Directories: %d \n",globaldirNum);
      printf(" Regular files: %d \n",globalregFiles);
      printf(" Special files: %d \n",globalspecialFiles);
      printf(" Regular files bytes: %lld \n",globalbytes);
      printf(" Text files: %d \n",globaltextnum);
      printf(" Text files bytes: %lld \n",globaltextBytes);


      sem_destroy(&sem1);


      printf("end of program\n");
      exit(0);


  }
