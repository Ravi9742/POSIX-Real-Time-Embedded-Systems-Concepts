/**
 * Ravi Agarwal
 * 
 * Special thanks to Dr. Sam Siewert for the code refernce https://github.com/siewertsmooc/RTES-ECEE-5623
 */
// Include necessary headers for threads, I/O, and system logging, posix functions
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sched.h>
#include <syslog.h>
#include <unistd.h>

// Define number of threads to be created
#define NUM_THREADS 128

// Number of CPUs onthe hardware
#define NUM_CPUS 4

// Define a struct for passing parameters to threads
typedef struct
{
    int threadIdx;   // Thread Index
} threadParams_t;


// POSIX thread declarations and scheduling attributes
pthread_t threads[NUM_THREADS];
pthread_t mainthread;
pthread_t startthread;
threadParams_t threadParams[NUM_THREADS];

pthread_attr_t fifo_sched_attr; // Thread attribute object for configuring FIFO real-time scheduling settings
pthread_attr_t orig_sched_attr; // Thread attribute object to hold the original/default scheduling settings
struct sched_param fifo_param;

#define SCHED_POLICY SCHED_FIFO
#define MAX_ITERATIONS (1000000)

/**
 * @brief Prints the current scheduling policy of the process
 *
 * SCHED_OTHER → Default time-sharing policy (normal processes)
 * SCHED_FIFO  → Real-time, first-in, first-out scheduling
 * SCHED_RR    → Real-time, round-robin scheduling
 *
 * @return void
 */
void print_scheduler(void)
{
    int schedType = sched_getscheduler(getpid());

    switch(schedType)
    {
        case SCHED_FIFO:
            printf("Pthread policy is SCHED_FIFO\n");
            break;
        case SCHED_OTHER:
            printf("Pthread policy is SCHED_OTHER\n");
            break;
        case SCHED_RR:
            printf("Pthread policy is SCHED_RR\n");
            break;
        default:
            printf("Pthread policy is UNKNOWN\n");
    }
}

/**
 * @brief Configure process and thread scheduling to real-time FIFO policy.
 *
 * This function performs the following steps:
 * 1. Prints the current scheduler policy using print_scheduler().
 * 2. Initializes a POSIX thread attribute object (fifo_sched_attr) for creating
 *    threads with explicit scheduling attributes.
 * 3. Sets the scheduling policy of threads to SCHED_FIFO (real-time FIFO).
 * 4. Sets CPU affinity for threads to run only on CPU core 3.
 * 5. Retrieves the maximum priority allowed for the SCHED_FIFO policy and sets
 *    the fifo_param structure accordingly.
 * 6. Sets the scheduler for the current process to SCHED_FIFO at maximum priority.
 * 7. Applies the priority parameter to the thread attribute object.
 * 8. Prints the adjusted scheduler policy using print_scheduler() to confirm changes.
 *
 * Notes:
 * - Requires root privileges to set SCHED_FIFO for the process.
 * - Threads created with fifo_sched_attr will inherit these scheduling and
 *   CPU affinity settings.
 *
 * @return void
 */
void set_scheduler(void)
{
    int max_prio, rc, cpuidx;
    cpu_set_t cpuset;

    printf("INITIAL "); print_scheduler();

    pthread_attr_init(&fifo_sched_attr);
    pthread_attr_setinheritsched(&fifo_sched_attr, PTHREAD_EXPLICIT_SCHED); // This call forces threads to use the explicitly set attributes
    pthread_attr_setschedpolicy(&fifo_sched_attr, SCHED_POLICY);
    CPU_ZERO(&cpuset); // clears the CPU set
    cpuidx = (3);      // choose CPU core #3 	 	
    CPU_SET(cpuidx, &cpuset); // adds CPU core #3 to the CPU set
    pthread_attr_setaffinity_np(&fifo_sched_attr, sizeof(cpu_set_t), &cpuset);

    max_prio = sched_get_priority_max(SCHED_POLICY); // Finds the maximum priority value allowed for SCHED_FIFO
    fifo_param.sched_priority = max_prio;    

    if((rc=sched_setscheduler(getpid(), SCHED_POLICY, &fifo_param)) < 0)
        perror("sched_setscheduler");

    pthread_attr_setschedparam(&fifo_sched_attr, &fifo_param);

    printf("ADJUSTED "); print_scheduler();
}



/**
 * @brief A counter thread function that adds the thread indices and logs the sum in syslog
 * 
 * @param threadp Pointer to threadParams_t
 * @return NULL
 */
void *counterThread(void *threadp)
{
    int sum=0, i, iterations;
    threadParams_t *threadParams = (threadParams_t *)threadp;
    double start=0.0, stop=0.0;
    struct timeval startTime, stopTime;

    gettimeofday(&startTime, 0);
    start = ((startTime.tv_sec * 1000000.0) + startTime.tv_usec)/1000000.0;


    for(iterations=0; iterations < MAX_ITERATIONS; iterations++)
    {
        sum=0;
        for(i=1; i < (threadParams->threadIdx)+1; i++)
            sum=sum+i;
    }


    gettimeofday(&stopTime, 0);
    stop = ((stopTime.tv_sec * 1000000.0) + stopTime.tv_usec)/1000000.0;

    printf("\nThread idx=%d, sum[0...%d]=%d, running on CPU=%d, start=%lf, stop=%lf", 
           threadParams->threadIdx,
           threadParams->threadIdx, sum, sched_getcpu(),
           start, stop);
		   
	syslog(LOG_INFO, "Thread idx=%d, sum[1...%d]=%d Running on core:%d",threadParams->threadIdx, threadParams->threadIdx, sum, sched_getcpu());
	
	// Clean exit for the thread
	pthread_exit(NULL);
}

/**
 * This function creates multiple other threads
 *
 */
void *starterThread(void *threadp)
{
   int i;

   printf("starter thread running on CPU=%d\n", sched_getcpu());

   for(i=0; i < NUM_THREADS; i++)
   {
       threadParams[i].threadIdx=i;

       pthread_create(&threads[i],   // pointer to thread descriptor
                      &fifo_sched_attr,     // use FIFO RT max priority attributes
                      counterThread, // thread function entry point
                      (void *)&(threadParams[i]) // parameters to pass in
                     );

   }

   for(i=0;i<NUM_THREADS;i++)
       pthread_join(threads[i], NULL);
   
   // Clean exit for the thread
   pthread_exit(NULL);

}


int main (int argc, char *argv[])
{
   int rc;
   int j;
   cpu_set_t cpuset;
   char buffer[1024]; // Buffer to store output of uname -a
   
   //execute uname -a and read output into the buffer 
   FILE* uname_output = popen("uname -a", "r");
   fgets(buffer, sizeof(buffer), uname_output);
   pclose(uname_output);

   syslog(LOG_INFO, "%s", buffer);
   
   
   set_scheduler();

   CPU_ZERO(&cpuset);

   // get affinity set for main thread
   mainthread = pthread_self();

   // Check the affinity mask assigned to the thread 
   rc = pthread_getaffinity_np(mainthread, sizeof(cpu_set_t), &cpuset);
   if (rc != 0)
       perror("pthread_getaffinity_np");
   else
   {
       printf("main thread running on CPU=%d, CPUs =", sched_getcpu());

       for (j = 0; j < CPU_SETSIZE; j++)
           if (CPU_ISSET(j, &cpuset))
               printf(" %d", j);

       printf("\n");
   }

   // Create the thread(s)
   pthread_create(&startthread,      // pointer to thread descriptor
                  &fifo_sched_attr,  // use FIFO RT max priority attributes
                  starterThread,     // thread function entry point
                  (void *)0          // parameters to pass in
                 );

   // Wait for thread to complete;	
   pthread_join(startthread, NULL);

   
}
