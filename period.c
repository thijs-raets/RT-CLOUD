#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sched.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>

#define HZ      2100076000
#define MHZ     2100076
#define UHZ     2100




typedef unsigned long long ticks;

typedef struct DF {
    ticks dispatch;
    ticks finish;
} DF;

int cost;
DF* data;
DF* current;
DF* limit;
int pid;

static inline ticks
getticks(void)
{
    unsigned a, d;

    asm("cpuid");
    asm volatile("rdtsc": "=a" (a), "=d" (d));

    return (((ticks)a) | ((ticks)d) << 32);
}

//workload for 1ms
static inline void
workload(void) {
    double temp = 0;
    long long i;
    
   	for (i = 0; i < 74000; i++) 
       	temp = sqrt((double)i*i);
}

static void
work(int sig, siginfo_t *extra, void *cruft)
{
    ticks temp;

    if(current < limit) {
        temp = getticks();
        current->dispatch = temp;
        int i;
        for (i = 0; i < cost; i++) {
            workload();
        }
        temp = getticks();
        current->finish = temp;
        ++current;
    }
}

static void
usage(void)
{
    fprintf(stderr, "Usage: period cost period core duration priority\n");
    exit(EXIT_FAILURE);
}

int 
main(int argc, char *argv[]) {
    if(argc != 6)
        usage();

    cost = atoi(argv[1]);       // Execution time (ms)
    long long period = atoi(argv[2]);     // Period (ms)
    int core = atoi(argv[3]);
    int duration = atoi(argv[4]);   // Duration of experiment (ms)
    int priority = atoi(argv[5]);
/*
    int h = 0;

    ticks now1 = getticks();
    for (h = 0; h < 1000; h++)
        workload();
    ticks spent = getticks();
    spent -= now1;
    printf("spent %lu times\n", spent/MHZ);

*/
    if(cost <= 0) {
        printf(stderr, "cost must be greater than zero\n");
        exit(EXIT_FAILURE);
    }
    if(period <= 0) {
        fprintf(stderr, "period must be greater than zero\n");
        exit(EXIT_FAILURE);
    }
    if(core != 1) {
        fprintf(stderr, "core must be 1\n");
        exit(EXIT_FAILURE);
    }
    if(duration <= 0) {
        fprintf(stderr, "duration must be greater than zero\n");
        exit(EXIT_FAILURE);
    }
    
    if(cost >= period) {
        fprintf(stderr, "cost must be less than period\n");
        exit(EXIT_FAILURE);
    }

    if(period >= duration) {
        fprintf(stderr, "period must be less than duration\n");
        exit(EXIT_FAILURE);
    }

    // Determine the number of times to run.
    int count = (duration - 1) / period + 1;
    
    /*unsigned long mask = core;
	//sched_setaffinity set the core for the task to run on
    if (sched_setaffinity(0, sizeof(mask), &mask) < 0) {
        perror("sched_setaffinity");
        exit(EXIT_FAILURE);
    }*/

    /*struct sched_param sched;
    sched.sched_priority = priority;
    if (sched_setscheduler(getpid(), SCHED_FIFO, &sched) < 0) {
        perror("sched_setscheduler");
        exit(EXIT_FAILURE);
    }*/
	//Allocates a block of size bytes of memory, returning a pointer to the beginning of the block.
    if((data = malloc (count * sizeof(DF))) == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    current = data;
    limit = data + count;
    pid = getpid();
	//Sets the first num(count* sizeof(DF)) bytes of the block of memory pointed by data to the specified value(-1) (interpreted as an unsigned char).
    memset(data, -1, count* sizeof(DF));
	
    if (mlock(data, count * sizeof(DF)) < 0) {
        perror("mlock");
        exit(EXIT_FAILURE);
    }
	//alter program behaviour on receiving OS signal
    struct sigaction sa;
	//sigemptyset manipulates signal sets. Signal sets are data objects that let a thread keep track of groups of signals
    //sigemptyset() initializes the signal set specified by set to an empty set.
	sigemptyset(&sa.sa_mask);
	//If the SA_SIGINFO flag is set in the sa_flags field, the sa_sigaction field specifies a signal-catching function.
    sa.sa_flags = SA_SIGINFO;
	//set the Signal-catching function.
    sa.sa_sigaction = work;

    if (sigaction(SIGRTMIN, &sa, NULL) < 0) {
        perror("sigaction error");
        exit(EXIT_FAILURE);
    }
	//contains two structs with each nano second time and normal time, one for interval and one for value
    struct itimerspec timerspec;
    
    timerspec.it_interval.tv_sec = period / 1000;
    timerspec.it_interval.tv_nsec = (period % 1000) * 1000000;

    ticks start_time = getticks() + HZ;    
    struct timespec now;
    if(clock_gettime(CLOCK_REALTIME, &now) < 0) {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }

    // Start one second from now.
    timerspec.it_value.tv_sec = now.tv_sec + 1;
    timerspec.it_value.tv_nsec = now.tv_nsec;

	
	// sigevent - structure for notification from asynchronous routines
    struct sigevent timer_event;
    timer_t timer;
	// sigev_notify;  Notification method 
    // sigev_signo;   Notification signal 
    timer_event.sigev_notify = SIGEV_SIGNAL;
    timer_event.sigev_signo = SIGRTMIN;
    timer_event.sigev_value.sival_ptr = (void *)&timer;

    if (timer_create(CLOCK_REALTIME, &timer_event, &timer) < 0) {
        perror("timer_create");
        exit(EXIT_FAILURE);
    }


    if (timer_settime(timer, TIMER_ABSTIME, &timerspec, NULL) < 0) {
        perror("timer_settime");
        exit(EXIT_FAILURE);
    }
    
    sigset_t allsigs;
    sigemptyset(&allsigs);

    DF* limit = data + count;
	//This function replaces the current signal mask for the calling process 
	//with a new signal set and then suspends the calling process until a signal is delivered.
    while(current < limit) {
        sigsuspend(&allsigs);
    }
	
	//if false program execution is terminated
    assert(data + count == current);

    current = data;
    int i;
    for(i = 0; i < count; ++i) {
        printf("%lld %lld %lld %lld\n", start_time, current->dispatch, current->finish, start_time + period*MHZ);
//        printf("%lld %lld %lld\n", (current->dispatch - start_time) / UHZ, (current->finish - current->dispatch) / UHZ, (start_time + period*MHZ - current->finish) / UHZ);
        start_time += period*MHZ;
        ++current;
    }
    
    int j;	
    for(j = 0; j < 10; ++j) {
        printf("\n");
    }



    if (munlock(data, count * sizeof(DF)) < 0) {
        perror("munlock");
        exit(EXIT_FAILURE);
    }

    free(data);
    
    exit(EXIT_SUCCESS);
}

