#include <errno.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#include "shared_defs.h"

int main(void)
{
	int logq_id, msgrcv_flags, cntr;
	struct rt_logbuf shmem_msg_log;

	logq_id = msgget(LOGQ_KEY, IPC_CREAT | 0660);
	if (logq_id < 0) {
		perror("error attaching logging message queue");
		return 1;
	}

	for (cntr = 0;; cntr++) {
		int ret;

		/* first check if there is a message in the queue */
		/* if it's empty we have time for fflush */
		msgrcv_flags = MSG_NOERROR | IPC_NOWAIT;
		do {
			ret = msgrcv(logq_id, (struct msgbuf *)&shmem_msg_log,
					sizeof(shmem_msg_log) - sizeof(shmem_msg_log.mtype),
					0, msgrcv_flags);

			if (ret < 0) {
				/* queue empty - flush output */
				if (errno == ENOMSG && (msgrcv_flags & IPC_NOWAIT)) {
					fflush(stdout);
				} else {
					if (errno != EINTR)
						perror("error upon IPC message receive");
				}
			}

			msgrcv_flags &= ~IPC_NOWAIT;
		} while (ret < 0);

		printf("\t0:\t%d:\t%s%d.%03d\n", cntr, shmem_msg_log.negative ? "-" : "",
				shmem_msg_log.timediff.tv_sec * 1000000UL +
				shmem_msg_log.timediff.tv_nsec / 1000,
				shmem_msg_log.timediff.tv_nsec % 1000);
	}
}

/* vim: set cindent ts=8 sts=8 */
