#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#include "shared_defs.h"

int main(void)
{
	int logq_id, cntr;
	struct rt_logbuf shmem_msg_log;

	logq_id = msgget(LOGQ_KEY, IPC_CREAT | 0660);
	if (logq_id < 0) {
		perror("error attaching logging message queue");
		return 1;
	}

	for (cntr = 0;; cntr++) {
		int ret;

		/* we take any message from the queue regardless its length */
		ret = msgrcv(logq_id, (struct msgbuf *)&shmem_msg_log,
				sizeof(shmem_msg_log) - sizeof(shmem_msg_log.mtype),
				0, MSG_NOERROR);
		if (ret < 0)
			perror("error upon IPC message receive");

		printf("\t0:\t%d:\t%s%d.%03d\n", cntr, shmem_msg_log.negative ? "-" : "",
				shmem_msg_log.timediff.tv_sec * 1000000UL +
				shmem_msg_log.timediff.tv_nsec / 1000,
				shmem_msg_log.timediff.tv_nsec % 1000);
	}
}

/* vim: set cindent ts=8 sts=8 */
