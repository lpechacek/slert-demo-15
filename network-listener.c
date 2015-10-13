#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/udp.h>

#include "shared_defs.h"

int main(void)
{
	int udp_socket;
	struct sockaddr_in udp_sockaddr;
	unsigned char msg_buffer[MSG_SIZE];

	/* TODO bind to shared memory */
	/* TODO parse consumer PID from command line */

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
		ret = recv(udp_socket, &msg_buffer, sizeof(msg_buffer), 0);
		if (ret < 0)
			perror("error upon receive");

		/* TODO place data into shared memory and signal consumer */
	}
}

/* vim: ci ts=8 sts=8 */
