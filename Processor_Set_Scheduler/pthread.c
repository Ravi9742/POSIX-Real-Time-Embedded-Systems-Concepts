/**
 * Ravi Agarwal
 * 
 * 
 */
#include <sched.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

// choose the scheduler policy
#define SCHED_POLICY SCHED_FIFO

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

/**
 * @brief Sets the scheduling policy according to the macro SCHED_POLICY
 *
 * SCHED_OTHER → Default time-sharing policy (normal processes)
 * SCHED_FIFO  → Real-time, first-in, first-out scheduling
 * SCHED_RR    → Real-time, round-robin scheduling
 *
 * @return void
 */
void set_scheduler(void)
{
    int max_prio, rc; 
    struct sched_param sp;

    max_prio = sched_get_priority_max(SCHED_POLICY); // Finds the maximum priority value allowed
    sp.sched_priority = max_prio; 

    rc = sched_setscheduler(getpid(), SCHED_POLICY, &sp);
    if (rc == -1){
        perror("sched_setscheduler");
    }



}

int main (int argc, char *argv[])
{
    printf("INITIAL "); print_scheduler();
    set_scheduler();
    printf("ADJUSTED "); print_scheduler();

}