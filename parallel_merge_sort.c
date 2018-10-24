/*********************************************************************
* Programmer: Zachary Mericle                                        *
* Last Edited: 10/14/18                                              *
*                                                                    *
**********************************************************************/

/* This program creates a producer thread and consumer that
   write to and read from a shared character buffer
*/

#define _REENTRANT
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdlib.h>

/*Key number */
#define SHMKEY ((key_t) 9999)

/* Shared structure with buffer */
typedef struct
{
  int integer_array[];
} shared_mem;

pthread_t       tids[];
pthread_attr_t  attr[1]; //Attribute pointer array
shared_mem *sh_mem;
sem_t mutex;

/*
int main(){
  //Initialize necessary variables for shared memory
  int shmid;
  char * shmadd;
  shmadd = (char *) 0;

  //Initialize the three semaphores
  sem_init(&mutex, 0, 1);
  sem_init(&full, 0, 0);
  sem_init(&empty, 0, 15);

  //Create a shared memory section
  if ((shmid = shmget(SHMKEY, 15*sizeof(char), IPC_CREAT | 0666)) < 0){
    perror("shmget");
    exit(1);
  }

  //Connect to shared memory section
  if ((sh_mem = (shared_mem *) shmat(shmid, shmadd, 0)) == (shared_mem *)-1){
    perror("shmat");
    exit(0);
  }

  //Schedule thread independently
  pthread_attr_init(&attr[0]);
  pthread_attr_setscope(&attr[0], PTHREAD_SCOPE_SYSTEM);
  //Schedule thread independently END

  //Create threads
  pthread_create(&tid1[0], &attr[0], producer, NULL);
  pthread_create(&tid2[0], &attr[0], consumer, NULL);

  //Wait for threads to finish
  pthread_join(tid1[0], NULL);
  pthread_join(tid2[0], NULL);

  //Terminate semaphores
  sem_destroy(&mutex);

  //Detach and remove shared memory
  if (shmdt(sh_mem) == -1){
    perror("shmdt");
    exit(-1);
  }
  shmctl(shmid, IPC_RMID, NULL);

//Terminate threads
  pthread_exit(NULL);
}

*/
