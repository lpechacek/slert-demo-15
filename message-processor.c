#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/user.h>

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

int main(int argc, char **argv)
{
	int msgq_id, logq_id;
	int opt, file_size = DFL_FILE_SIZE, shared_file = -1;
	char *file_name = NULL;
	struct rt_msgbuf shmem_msg;
	struct rt_logbuf shmem_msg_log;
	struct timespec ts_now;


	while ((opt = getopt(argc, argv, "f:s:")) != -1) {
		switch (opt) {
			case 'f':
				file_name = optarg;
				break;
			case 's':
				file_size = atoi(optarg);
				break;
			default:
				return 1;
		}
	}

	if (file_size <= 0) {
		fputs("file size must be positive\n", stderr);
		return 1;
	}

	msgq_id = msgget(MSGQ_KEY, IPC_CREAT | 0660);
	if (msgq_id < 0) {
		perror("error attaching the IPC message queue");
		return 1;
	}

	logq_id = msgget(LOGQ_KEY, IPC_CREAT | 0660);
	if (msgq_id < 0) {
		perror("error attaching logging message queue");
		return 1;
	}

	if (file_name) {
		shared_file = open(file_name, O_RDWR, S_IRUSR|S_IWUSR);

		if (shared_file < 0) {
			perror("error opening shared file");
			return 1;
		}
	}

	for(;;) {
		int ret;
		/* we take any message from the queue regardless its length */
		ret = msgrcv(msgq_id, (struct msgbuf *)&shmem_msg,
				sizeof(shmem_msg) - sizeof(shmem_msg.mtype),
				0, MSG_NOERROR);
		if (ret < 0)
			perror("error upon IPC message receive");

		if (shared_file >= 0) {
			ret = fallocate(shared_file, FALLOC_FL_KEEP_SIZE|FALLOC_FL_PUNCH_HOLE,
					random() % (file_size - PAGE_SIZE - 1) , PAGE_SIZE);
			if (ret < 0) {
				if (errno == EOPNOTSUPP) {
					fputs("underlying file system doesn't "
							"support FALLOC_FL_PUNCH_HOLE - see fallocate(2)\n"
							"disabling file operations\n",
							stderr);
					close(shared_file);
					shared_file = -1;
				}
				perror("error upon punching hole");
			}
		}

		ret = clock_gettime(CLOCK_MONOTONIC, &ts_now);
		if (ret < 0)
			perror("error upon clock_gettime");

		shmem_msg_log.mtype = MSGQ_MSG_LOG;
		shmem_msg_log.negative = timespec_substract(&shmem_msg_log.timediff,
				&ts_now, &shmem_msg.recv_ts);

		ret = msgsnd(logq_id, (struct msgbuf *)&shmem_msg_log,
				sizeof(shmem_msg_log) - sizeof(shmem_msg_log.mtype),
				IPC_NOWAIT);
		if (ret < 0)
			perror("error upon log message send");
	}
}

/* vim: set cindent ts=8 sts=8 */
