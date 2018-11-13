#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

/*Key number */
#define SHMKEY ((key_t) 9999)

/* Shared structure with buffer */
typedef struct
{
  int *integer_array;
} shared_mem;

struct args
{
  int minIndex;
  int maxIndex;
  int threadIndex;
};

void normalMergeSort(int[], int);
void *mergeSort(void*);
void merge(int, int, int);
double report_running_time(char[], int);

pthread_attr_t  attr[1]; //Attribute pointer array
shared_mem *sh_mem;
pthread_t *tids;

struct timezone Idunno;
struct timeval startTime, endTime;

void normalMergeSort(int arr[], int arr_size)
{
  if(arr_size > 1)
  {
    int mid = arr_size/2;

    int remainder = arr_size%2;

    if(remainder == 1)
    {
      mid = mid + 1;
    }

    int lefthalf[mid];
    int righthalf[arr_size-mid];
    int a;
    int b;

    /*Stores the lowerhalf of the array into the lefthalf array*/
    for(a = 0; a < mid; a++)
    {
      lefthalf[a] = arr[a];
    }

    int x = 0;

    /*Stores the upperhalf of the array into the righthalf array*/
    for(b = mid; b < arr_size; b++)
    {
      righthalf[x] = arr[b];
      x++;
    }

    /*Recurive call for normalMergeSort of the lefthalf*/
    normalMergeSort(lefthalf, mid);

    /*Recurive call for normalMergeSort of the righthalf*/
    normalMergeSort(righthalf, arr_size/2);

    int i = 0;
    int j = 0;
    int k = 0;

    /*Compares the elements from the lefthalf and righthalf array, and sorts them before placing them into the arr array */
    while (i < mid && j < arr_size-mid)
    {
        if (lefthalf[i] <= righthalf[j])
        {
            arr[k] = lefthalf[i];
            i++;
        }
        else
        {
            arr[k] = righthalf[j];
            j++;
        }
        k++;
    }

    /*Copies the remaining elements of the lefthand array, if any*/
    while (i < mid)
    {
        arr[k] = lefthalf[i];
        i++;
        k++;
    }

    /*Copies the remaining elements of the righthand array, if any*/
    while (j < arr_size-mid)
    {
        arr[k] = righthalf[j];
        j++;
        k++;
    }
  }
}

void* mergeSort(void *input)
{
	// Designed as per our whiteboard discussion last week
  int maxIndex = ((struct args *)input)->maxIndex;
  int minIndex = ((struct args *)input)->minIndex;
  int threadIndex = ((struct args *)input)->threadIndex;
    //printf("entering mergesort parallel\n");
    if (((maxIndex - minIndex) + 1) < 2){
      //printf("Returning\n");
        return;
    }
	// 1.) Determine midpoint
    int midpoint = floor((minIndex+maxIndex)/2);
    //printf("midpoint calculated \n");
	// 2.) Find ranges for left half, right half:
	int leftThreadIndex = 2*threadIndex+1;
  int rightThreadIndex = leftThreadIndex + 1;

	// 3.) Create a thread for each half in array

  //printf("left create %d min:%d max:%d\n", leftThreadIndex, minIndex, midpoint);
  struct args *leftInput = (struct args *)malloc(sizeof(struct args));
  struct args *rightInput = (struct args *)malloc(sizeof(struct args));

  leftInput->minIndex = minIndex;
  leftInput->maxIndex = midpoint;
  leftInput->threadIndex = leftThreadIndex;

  rightInput->minIndex = midpoint+1;
  rightInput->maxIndex = maxIndex;
  rightInput->threadIndex = rightThreadIndex;

	pthread_create(&tids[leftThreadIndex], &attr[0], mergeSort, (void *)leftInput);
  //printf("right created %d min:%d max:%d\n", rightThreadIndex, midpoint+1, maxIndex);
  pthread_create(&tids[rightThreadIndex], &attr[0], mergeSort, (void *)rightInput);

  //printf("Waiting\n");
    // 4.) Wait for return
	pthread_join(tids[leftThreadIndex], NULL);
	pthread_join(tids[rightThreadIndex], NULL);

	// 5.) merge
    //printf("merge called\n");
    merge(minIndex, midpoint, maxIndex);

}

void merge(int min, int mid, int max){
  //Local array to store sorted array
  //printf("merge_arrsize\n");
  int merge_arr[(max-min)+1];

  //Calculated lengths of left and right arrays
  int left_len = mid - min + 1;
  int right_len = max - mid;

  //Arrays to hold left and right values respectively
  int left_arr[left_len];
  int right_arr[right_len];

  //printf("store values from sh mem into local \n");
  //Store values from shared memory for the set ranges
  for (int i = 0; i < left_len; i++){
    left_arr[i] = sh_mem->integer_array[i + min];
  }
  for (int j = 0; j < right_len; j++) {
	  right_arr[j] = sh_mem->integer_array[mid + j + 1];
  }
  //printf("iterators for each\n");
  //Iterators for each array
  int left_iter = 0;
  int right_iter = 0;
  int merge_iter = 0;

  //printf("begin merge \n");
  //Begin merge of left and right arrays
  while (left_iter < left_len && right_iter < right_len) {

	  if (left_arr[left_iter] <= right_arr[right_iter])
	  {
		  merge_arr[merge_iter] = left_arr[left_iter];
		  left_iter++;
	  }
	  else
	  {
		  merge_arr[merge_iter] = right_arr[right_iter];
		  right_iter++;
	  }
	  merge_iter++;
  }
  //Copy remaining elements if any into left array
  while (left_iter < left_len) {
	  merge_arr[merge_iter] = left_arr[left_iter];
	  left_iter++;
	  merge_iter++;
  }
  //Copy remaining elements if any into right array
  while (right_iter < right_len) {
	  merge_arr[merge_iter] = right_arr[right_iter];
	  right_iter++;
	  merge_iter++;
  }

  //printf("transfer\n");
  //Transfer sorted array into array in shared memory
  for (int i = min, j = 0; i < max+1; i++, j++){
    sh_mem->integer_array[i] = merge_arr[j];
  }

}



/*CONTAINS SAMPLE MAIN WITH MULTIPLE ARRAYS FOR CHECKING NORMALMERGESORT
int main()
{
  //int arr[] = {12, 11, 13, 5, 6, 7};
  //int arr[] = {33, 12, 53, 4};
  //int arr[] = {100, 20, 12, 61, 89, 90, 40,71, 200};
  //int arr[] = {8, 403, 17, 15, 49, 10, 0, 15, 30};
  int arr[] = {60, 32, 1, 0, 3, 200, 1000, 4, 10, 19, 6000, 90, 2};
  int arr_size = sizeof(arr)/sizeof(arr[0]);

  int i;

  printf("Original Array: ");
  for(i = 0; i < arr_size; i++)
    printf("%d ", arr[i]);
  normalMergeSort(arr, arr_size);
  printf("\nSorted Array: ");
  for(i = 0; i < arr_size; i++)
    printf("%d ", arr[i]);
  printf("\n");
  return 0;
}*/

double report_running_time(char type[], int size) {
	long sec_diff, usec_diff;
	gettimeofday(&endTime, &Idunno);
	sec_diff = endTime.tv_sec - startTime.tv_sec;
	usec_diff= endTime.tv_usec-startTime.tv_usec;
	if(usec_diff < 0) {
		sec_diff --;
		usec_diff += 1000000;
	}
	printf("Running time for %s Mergesort of size %d: %ld.%06ld\n", type, size, sec_diff, usec_diff);
	return (double)(sec_diff*1.0 + usec_diff/1000000.0);
}



int main(int argc, char *argv[]){
  //Initialize necessary variables for shared memory
  int shmid;
  int array_size = atoi(argv[1]);
  char * shmadd;
  //printf("Thread creation\n");
  int thread_size = (int) ceil(array_size*(log10(array_size)/log10(2)));
  //printf("thread_size %d \n", thread_size);
  tids = malloc(sizeof(pthread_t)*thread_size);
  int int_array[array_size];
  //printf("sh_mem\n");
  time_t t;

  //printf("srand\n");

  srand((unsigned) time(&t));
  shmadd = (char *) 0;

  //printf("Shm creating... \n");
  //Create a shared memory section
  if ((shmid = shmget(SHMKEY, array_size*sizeof(int), IPC_CREAT | 0666)) < 0){
    perror("shmget");
    exit(1);
  }
  //printf("Connecting to shm\n");
  //Connect to shared memory section
  if ((sh_mem = (shared_mem *) shmat(shmid, shmadd, 0)) == (shared_mem *)-1){
    perror("shmat");
    exit(0);
  }

  sh_mem->integer_array = malloc(sizeof(int) * array_size);

  //printf("Randomizing\n");
  for (int i = 0; i < array_size; i++){
    int random_integer = rand()%100;
    int_array[i] = random_integer;
    sh_mem->integer_array[i] = random_integer;
  }

  //Schedule thread independently
  pthread_attr_init(&attr[0]);
  pthread_attr_setscope(&attr[0], PTHREAD_SCOPE_SYSTEM);
  //Schedule thread independently END

  //printf("gettimeofday\n");
  gettimeofday(&startTime, &Idunno);

  normalMergeSort(int_array, array_size);
  report_running_time("Normal", array_size);
  //printf("\nSorted Array: ");
  /*for(int i = 0; i < array_size; i++)
    printf("%d ", int_array[i]);
  printf("\n");*/

  //printf("gettimeofday parallel\n");
  gettimeofday(&startTime, &Idunno);

  //printf("create thread mergesort \n");
  struct args *indices = (struct args *)malloc(sizeof(struct args));
  indices->minIndex = 0;
  indices->maxIndex = array_size-1;
  indices->threadIndex = 0;
  pthread_create(&tids[0], &attr[0], mergeSort, (void *)indices);


  pthread_join(tids[0], NULL);

  report_running_time("parallel", array_size);

  /*for(int i = 0; i < array_size; i++)
    printf("%d ", sh_mem->integer_array[i]);
  printf("\n");*/

  //Detach and remove shared memory
  if (shmdt(sh_mem) == -1){
    perror("shmdt");
    exit(-1);
  }
  shmctl(shmid, IPC_RMID, NULL);
//Terminate threads
  pthread_exit(NULL);
}
