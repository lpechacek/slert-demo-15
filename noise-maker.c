#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/user.h>

#include "shared_defs.h"

int main(int argc, char **argv)
{
	int shared_file, opt;
	int n_holes, ret;
	int file_size = DFL_FILE_SIZE;

	while ((opt = getopt(argc, argv, "s:")) != -1) {
		switch (opt) {
			case 's':
				file_size = atoi(optarg);
				break;
			default:
				return 1;
		}
	}

	if (!argv[optind]) {
		fputs("need file name as first parameter\n", stderr);
		return 1;
	}

	shared_file = open(argv[optind], O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);

	if (shared_file < 0) {
		perror("error creating shared file");
		return 1;
	}

	ret = fallocate(shared_file, 0, 0, file_size);
	if (ret < 0)
		perror("error upon allocating disk space");

	for (;;) {
		for (n_holes = 0; n_holes < 200; n_holes++) {
			ret = fallocate(shared_file, FALLOC_FL_KEEP_SIZE|FALLOC_FL_PUNCH_HOLE,
					random() % (file_size - PAGE_SIZE - 1) , PAGE_SIZE);
			if (ret < 0) {
				if (errno == EOPNOTSUPP) {
					fputs("underlying file system doesn't "
						"support FALLOC_FL_PUNCH_HOLE - see fallocate(2)\n",
						stderr);
					return 1;
				}
				perror("error upon punching hole");
			}
		}
	}
}

/* vim: set cindent ts=8 sts=8 */
