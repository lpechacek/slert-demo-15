#include <time.h>

#define UDP_PORT 1492
#define UDP_MSG_SIZE 64

#define MSGQ_KEY 0x53555345L
#define MSGQ_MSG_TS 1

struct rt_msgbuf {
	long mtype;
	struct timespec snd_ts;
	struct timespec recv_ts;
};

/* vim: set cindent ts=8 sts=8 */
