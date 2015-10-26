#include <assert.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "shared_defs.h"

#define xstr(s) str(s)
#define str(s) #s

int main(int argc, char **argv)
{
	int opt, count, max_count = 0;
	int udp_socket, ret;
	struct addrinfo *result;
	struct addrinfo resolver_hints = {
		.ai_family = AF_INET,
		.ai_socktype = SOCK_DGRAM,
		.ai_protocol = IPPROTO_UDP
	};
	uint32_t dest_addr;
	unsigned char udp_msg_buffer[UDP_MSG_SIZE];

	assert(UDP_MSG_SIZE > sizeof(struct timespec));

	while ((opt = getopt(argc, argv, "n:")) != -1) {
		switch (opt) {
			case 'n':
				max_count = atoi(optarg);
				break;
			default:
				return 1;
		}
	}

	if (!argv[optind]) {
		fputs("need target address as first parameter", stderr);
		return 1;
	}
	
	ret = getaddrinfo(argv[optind], xstr(UDP_PORT), &resolver_hints, &result);
	if (ret != 0) {
		if (ret == EAI_SYSTEM) {
			perror("getaddrinfo");
		} else {
			fprintf(stderr, "error in getaddrinfo: %s\n", gai_strerror(ret));
		}
		return 1;
	}

	udp_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (udp_socket < 0) {
		perror("error creating udp socket");
		return 1;
	}

	if (result->ai_next)
		fputs("more than one address records found, using the first one", stderr);

	if (connect(udp_socket, result->ai_addr, result->ai_addrlen)) {
		perror("error connecting udp socket");
		return 1;
	}

	for(count = 0; !max_count || count < max_count; count++) {
		ret = clock_gettime(CLOCK_MONOTONIC, (struct timespec *)&udp_msg_buffer);
		if (ret < 0)
			perror("error upon clock_gettime");

		ret = send(udp_socket, &udp_msg_buffer, sizeof(udp_msg_buffer), 0);
		if (ret < 0)
			perror("error upon UDP send");
	}
}

/* vim: set cindent ts=8 sts=8 */
