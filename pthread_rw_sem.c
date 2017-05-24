#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <semaphore.h>

#define handle_error_en(en, msg) \
    do { errno = en; perror(msg); exit(EXIT_FAILURE); } while(0)
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while(0)

char buf[1024] = { 0 };

sem_t sem_w;			/* 写资源 */
sem_t sem_r;			/* 读资源 */

/* 初始化 */
int init_sem(sem_t * sem, unsigned int value)
{
	if (sem_init(sem, 0, value) < 0) {
		perror("sem_init");
		exit(EXIT_FAILURE);
	}

	return 0;
}

/* P操作 */
int P(sem_t * sem)
{
	if (sem_wait(sem) < 0) {
		perror("sem_wait");
		exit(EXIT_FAILURE);
	}

	return 0;
}

/* V操作 */
int V(sem_t * sem)
{
	if (sem_post(sem) < 0) {
		perror("sem_post");
		exit(EXIT_FAILURE);
	}

	return 0;
}

void *write_thread(void *arg)
{
	while (1) {
		P(&sem_w);

		printf(">");
		fgets(buf, sizeof(buf), stdin);

		V(&sem_r);
		if (strncmp(buf, "quit", 4) == 0)
			pthread_exit(NULL);
		printf("%d:%s\n", __LINE__, __FUNCTION__);
	}
}

void *read_thread(void *arg)
{
	while (1) {
		P(&sem_r);	/* 申请资源 */
		fputs(buf, stdout);

		if (strncmp(buf, "quit", 4) == 0)
			pthread_exit(NULL);
		printf("%d:%s\n", __LINE__, __FUNCTION__);
		V(&sem_w);	/* 释放资源 */
	}
}

int main(int argc, const char *argv[])
{
	pthread_t tid[2];
	int ret = 0;

	/* 初始化信号量 */
	init_sem(&sem_w, 1);
	init_sem(&sem_r, 0);

	if ((ret = pthread_create(&tid[0], NULL, read_thread, NULL)) != 0) {
		handle_error_en(ret, "pthread_create");
	}

	if ((ret = pthread_create(&tid[1], NULL, write_thread, NULL)) != 0) {
		handle_error_en(ret, "pthread_create");
	}

	/* 等待线程退出 */
	pthread_join(tid[0], NULL);
	pthread_join(tid[0], NULL);
	return 0;
}
