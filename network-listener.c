#include <stdio.h>
#include <string.h>
#include <time.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "shared_defs.h"

int main(void)
{
	int udp_socket, msgq_id;
	struct sockaddr_in udp_sockaddr;
	unsigned char udp_msg_buffer[UDP_MSG_SIZE];
	struct rt_msgbuf shmem_msg;

	msgq_id = msgget(MSGQ_KEY, IPC_CREAT | 0660);
	if (msgq_id < 0) {
		perror("error attaching the IPC message queue");
		return 1;
	}

	udp_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (udp_socket < 0) {
		perror("error creating udp socket");
		return 1;
	}

	memset(&udp_sockaddr, 0, sizeof(udp_sockaddr));
	udp_sockaddr.sin_family = AF_INET;
	udp_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	udp_sockaddr.sin_port = htons(UDP_PORT);

	if (bind(udp_socket, (struct sockaddr *)&udp_sockaddr, sizeof(udp_sockaddr))) {
		perror("error binding udp socket");
		return 1;
	}

	for(;;) {
		int ret;
		ret = recv(udp_socket, &udp_msg_buffer, sizeof(udp_msg_buffer), 0);
		if (ret < 0)
			perror("error upon receive");

		shmem_msg.mtype = MSGQ_MSG_TS;
		ret = clock_gettime(CLOCK_MONOTONIC, &shmem_msg.recv_ts);
		if (ret < 0)
			perror("error upon clock_gettime");
		memcpy(&shmem_msg.snd_ts, &udp_msg_buffer, sizeof(shmem_msg.snd_ts));

		ret = msgsnd(msgq_id, (struct msgbuf *)&shmem_msg,
				sizeof(shmem_msg) - sizeof(shmem_msg.mtype),
				IPC_NOWAIT);
		if (ret < 0)
			perror("error upon IPC message send");
	}
}

/* vim: set cindent ts=8 sts=8 */
