/**
 * Ravi Agarwal
 * 
 * 
 */
#include <sched.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>


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
            printf("Policy is SCHED_FIFO\n");
            break;
        case SCHED_OTHER:
            printf("Policy is SCHED_OTHER\n");
            break;
        case SCHED_RR:
            printf("Policy is SCHED_RR\n");
            break;
        case -1:
            perror("sched_getscheduler");
            break; 
        default:
            printf("Policy is UNKNOWN\n");
    }
}
int main (int argc, char *argv[])
{
    print_scheduler();

}