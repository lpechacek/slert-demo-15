#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#include "shared_defs.h"

/* Subtract the ‘struct timespec’ values X and Y, storing the result in RESULT.
   Return 1 if the difference is negative, otherwise 0. */

int timespec_substract(struct timespec *result, struct timespec *x, struct timespec *y)
{
	/* Perform the carry for the later subtraction by updating y. */
	if (x->tv_nsec < y->tv_nsec) {
		int nsec = (y->tv_nsec - x->tv_nsec) / 1000000000 + 1;
		y->tv_nsec -= 1000000000 * nsec;
		y->tv_sec += nsec;
	}
	if (x->tv_nsec - y->tv_nsec > 1000000000) {
		int nsec = (x->tv_nsec - y->tv_nsec) / 1000000000;
		y->tv_nsec += 1000000000 * nsec;
		y->tv_sec -= nsec;
	}

	/* Compute the time remaining to wait.
	   tv_nsec is certainly positive. */
	result->tv_sec = x->tv_sec - y->tv_sec;
	result->tv_nsec = x->tv_nsec - y->tv_nsec;

	/* Return 1 if result is negative. */
	return x->tv_sec < y->tv_sec;
}

int main(void)
{
	int msgq_id;
	struct rt_msgbuf shmem_msg;
	struct timespec ts_now, timediff;

	msgq_id = msgget(MSGQ_KEY, IPC_CREAT | 0660);
	if (msgq_id < 0) {
		perror("error attaching the IPC message queue");
		return 1;
	}

	for(;;) {
		int ret;
		/* we take any message from the queue regardless its length */
		ret = msgrcv(msgq_id, (struct msgbuf *)&shmem_msg,
				sizeof(shmem_msg) - sizeof(shmem_msg.mtype),
				0, MSG_NOERROR);
		if (ret < 0)
			perror("error upon IPC message receive");

		ret = clock_gettime(CLOCK_MONOTONIC, &ts_now);
		if (ret < 0)
			perror("error upon clock_gettime");

		ret = timespec_substract(&timediff, &ts_now, &shmem_msg.recv_ts);

		/* TODO pass the result on to logging process */
		printf("%s%d.%09d\n", ret ? "-" : "", timediff.tv_sec, timediff.tv_nsec);
	}
}

/* vim: set cindent ts=8 sts=8 */
