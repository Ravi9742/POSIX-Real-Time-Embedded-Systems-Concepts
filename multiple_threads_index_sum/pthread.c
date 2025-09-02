/**
 * Ravi Agarwal
 * 
 * Special thanks to Dr. Sam Siewert for the code refernce https://github.com/siewertsmooc/RTES-ECEE-5623
 */

// Include necessary headers for threads, I/O, and system logging
#include <pthread.h>  
#include <stdlib.h>   
#include <stdio.h>    
#include <syslog.h>   

//Define number of threads to be created
#define NUM_THREADS 128

                      
// Define a struct for passing parameters to threads
typedef struct
{
    int threadIdx;  // Thread Index
} threadParams_t;


// Declare thread handles and parameter storage
pthread_t threads[NUM_THREADS];               
threadParams_t threadParams[NUM_THREADS];     

/**
 * @brief A counter thread function that adds the thread indices and logs the sum in syslog
 * 
 * @param threadp Pointer to threadParams_t, not used in this implementation
 * @return void* Always returns NULL
 */ 
void *counterThread(void *threadp)
{
    int sum=0, i;
    threadParams_t *threadParams = (threadParams_t *)threadp;

    for(i=1; i < (threadParams->threadIdx)+1; i++)
        sum=sum+i;

    // Log a message from the thread to the system log with CRIT level
    syslog(LOG_CRIT, "Thread idx=%d, sum[1...%d]=%d",threadParams->threadIdx, threadParams->threadIdx, sum);
    
	// Clean exit for the thread
    pthread_exit(NULL);
}


int main (int argc, char *argv[])
{

   int i;
   char buffer[1024]; // Buffer to store output of uname -a


   //execute uname -a and read output into the buffer 
   FILE* uname_output = popen("uname -a", "r");    
   fgets(buffer, sizeof(buffer), uname_output);    
   pclose(uname_output);                           
   
   // Log the uname -a output to syslog
   syslog(LOG_CRIT, "%s", buffer);

   // Create the thread(s)
   for(i=0; i < NUM_THREADS; i++)
   {
       threadParams[i].threadIdx=i;

       pthread_create(&threads[i],               // pointer to thread descriptor
                      (void *)0,                 // use default attributes
                      counterThread,             // thread function entry point
                      (void *)&(threadParams[i]) // parameters to pass in
                     );

   }

   
   // Wait for all threads to complete; one is this example
   for(i=0;i<NUM_THREADS;i++) 
       pthread_join(threads[i], NULL);  // Block until thread terminates

   return 0;  
}
