#include <time.h>

#define UDP_PORT 1492
#define UDP_MSG_SIZE 64

#define MSGQ_KEY 0x53555345L
#define MSGQ_MSG_TS 1

#define LOGQ_KEY 0x45535553L
#define MSGQ_MSG_LOG 1

#define DFL_FILE_SIZE (512<<20)

struct rt_msgbuf {
	long mtype;
	struct timespec snd_ts;
	struct timespec recv_ts;
};

struct rt_logbuf {
	long mtype;
	int negative;
	struct timespec timediff;
};

/* vim: set cindent ts=8 sts=8 */
