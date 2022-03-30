#include "stdio.h"
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <poll.h>

#define DMA_FENCE_IN_CMD		_IOWR('f', 0, int)
#define DMA_FENCE_OUT_CMD		_IOWR('f', 1, int)
#define DMA_FENCE_SIGNAL_CMD	_IO('f', 2)

#define DMA_FENCE_DEV "/dev/dma_fence_dev"
#define BLOCKING_IN_KERNEL

#define FD_NUM 10
int fd = -1;

static inline int sync_wait(int fd, int timeout)
{
	struct pollfd fds[FD_NUM] = {0};//可以检测10个fd，实际只检测了一个
	int ret;
	int poll_fd_num = 1;
	int i;

	for(i=0; i<FD_NUM; i++){
		fds[i].fd = -1;
	}
	
	fds[0].fd = fd;
	fds[0].events = POLLIN;

	do {
		ret = poll(fds, poll_fd_num, timeout);
		if (ret < 0) {
			printf("ERROR: errno=%d, revents=%d\n", errno, fds[0].revents);
			break;
		} else if (ret == 0){
			printf("sync_wait timeout for %ds\n", timeout);
		} else {
			printf("poll event occur, revents=%d\n", errno, fds[0].revents);
			break;
		}
	} while (1);
	return ret;
}

static void * signal_pthread(void *arg)
{
	sleep(3);

	if (ioctl(fd, DMA_FENCE_SIGNAL_CMD) < 0) {
		perror("get out fence fd fail\n");
	}

	return NULL;
}

int main(void)
{

	int out_fence_fd;
	pthread_t tidp;
	int ret=0;

    fd = open(DMA_FENCE_DEV, O_RDWR | O_NONBLOCK, 0);
	if (-1 == fd) {
		printf("Cannot open dma-fence dev =%s\n", DMA_FENCE_DEV);
		exit(1);
	}

	if(ioctl(fd, DMA_FENCE_OUT_CMD, &out_fence_fd) < 0) {
		perror("get out fence fd fail\n");
		close(fd);
		return -1;
	}

	printf("Get an out-fence fd = %d\n", out_fence_fd);

	if ((pthread_create(&tidp, NULL, signal_pthread, NULL)) == -1) {
		printf("create error!\n");
		close(out_fence_fd);
		close(fd);
		return -1;
	}

#ifdef BLOCKING_IN_KERNEL
	printf("Waiting out-fence to be signaled on KERNEL side ...\n");
	if(ioctl(fd, DMA_FENCE_IN_CMD, &out_fence_fd) < 0) {
		perror("get out fence fd fail\n");
		close(out_fence_fd);
		close(fd);
		return -1;
	}
#else
	printf("Waiting out-fence to be signaled on USER side ...\n");
	ret = sync_wait(out_fence_fd, -1);
	printf("sync_wait ret=%d\n", ret);
#endif

	printf("out-fence is signaled\n");

	if (pthread_join(tidp, NULL)) {
		printf("thread is not exit...\n");
		return -1;
	}

	close(out_fence_fd);
	close(fd);

    return 0;
}
