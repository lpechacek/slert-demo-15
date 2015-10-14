#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#include "shared_defs.h"

int main(void)
{
	int msgq_id;
	struct rt_msgbuf shmem_msg;

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

		/* TODO calculate times */
	}
}

/* vim: set cindent ts=8 sts=8 */
