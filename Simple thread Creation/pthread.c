/**
 * Ravi Agarwal
 * 
 * Special thanks to Dr. Sam Siewert for the code inspiration https://github.com/siewertsmooc/RTES-ECEE-5623
 */

// Include necessary headers for threads, I/O, and system logging
#include <pthread.h> 
#include <stdlib.h>  
#include <stdio.h>   
#include <syslog.h>  

//Define number of threads to be created
#define NUM_THREADS 1

                      
// Struct for passing parameters to threads (not unused here)
typedef struct
{
    int threadIdx; 
} threadParams_t;


// Declare thread handles and parameter storage
pthread_t threads[NUM_THREADS];            
threadParams_t threadParams[NUM_THREADS];  

/**
 * @brief A simple thread function that logs a message to syslog
 * 
 * @param threadp Pointer to threadParams_t, not used in this implementation
 * @return void* Always returns NULL
 */
void *simple_thread(void *threadp)
{ 
	(void)threadp; //thread parameters not used
	
	// Log a message from the thread to the system log with CRIT level
    syslog(LOG_CRIT, "Hello World from Thread!");

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

   // Log main thread "Hello World" message
   syslog(LOG_CRIT, "Hello World from Main!");

   // Create the thread(s)
   for(i=0; i < NUM_THREADS; i++)
   {
       threadParams[i].threadIdx=i;

       pthread_create(&threads[i],   // pointer to thread descriptor
                      (void *)0,     // no attributes
                      simple_thread, // Thread function entry point
                      (void *)0      // No argument passed to thread
                     );

   }

   
   // Wait for all threads to complete; one is this example
   for(i=0;i<NUM_THREADS;i++)
       pthread_join(threads[i], NULL); // Block until thread terminates

   
   return 0;
}
