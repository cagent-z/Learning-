#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define handle_error_en(en, msg) \
    do { errno = en; perror("msg"); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

/* 静态初始化一把锁 */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
/* 定义一个条件变量 */
pthread_cond_t cond;

char buf[1024] = { 0 };

void *write_thread(void *arg)
{
	while (1) {
		pthread_mutex_lock(&mutex);	/* 加锁 */
		printf(">");
		fgets(buf, sizeof(buf), stdin);

		pthread_cond_signal(&cond);	/* 发信号，使条件变量成立 */
		pthread_mutex_unlock(&mutex);	/* 释放锁 */
		if (strncmp(buf, "quit", 4) == 0)
			pthread_exit(NULL);
		usleep(100);
	}
}

void *read_thread(void *arg)
{
	while (1) {
		pthread_mutex_lock(&mutex);

		pthread_cond_wait(&cond, &mutex);
		fputs(buf, stdout);

		pthread_mutex_unlock(&mutex);
		if (strncmp(buf, "quit", 4) == 0)
			pthread_exit(NULL);
		usleep(100);
	}
}

int main(int argc, const char *argv[])
{
	pthread_t tid[2];
	int ret = 0;

	/* 动态初始化条件变量 */
	pthread_cond_init(&cond, NULL);
	if ((ret = pthread_create(&tid[0], NULL, read_thread, NULL)) != 0) {
		handle_error_en(ret, "pthread_create");
	}

	if ((ret = pthread_create(&tid[1], NULL, write_thread, NULL)) != 0) {
		handle_error_en(ret, "pthread_create");
	}

	/* 等待线程退出 */
	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);

	return 0;
}
