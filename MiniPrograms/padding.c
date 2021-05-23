/*
 * Capstone Design - Jin Hur, Yeongki Kweon, Jooyoung Kim - Dankook univ, Dept.SW
 * padding.c - A simple parallel program to demo false sharing (this uses padding to avoid false sharing)
 */

 //	compile as: gcc -DN=1000000 -D[GOOD|BAD] padding.c -lpthread -lrt -o padding
 //		
 //	run as: ./padding <numthreads>	


#include <pthread.h>	
#include <stdio.h>
#include <stdlib.h>	// for atoi() etc

#ifndef 	N
#define 	N		100000000
#endif
#ifndef 	REPEAT
#define 	REPEAT	1
#endif
#define		INTSIZE		4
#define		CACHELINE	64
#define 	PADDING 	(CACHELINE - INTSIZE)  // 쓰레드 간 거짓 공유를 방지하는 패딩 공간 크기
#define 	STATEBUFSZ 	8	// STATEBUFSZ=8 works; 16 OK/NOT and 32 NOT/OK for commit*/T420
#define 	MAXTHREADS 	CACHELINE/INTSIZE

#ifdef 	GOOD
#define 	MSG 	"# Padding: Good   : N=%d : Threads=%d \n"
#endif
#ifdef	BAD_FS
#define 	MSG 	"# Padding: Bad-FS : N=%d : Threads=%d \n"
#endif

void *thread_func(void *p);

typedef struct {
	int x;
#ifdef GOOD
	char padding[PADDING];  // GOOD 모드의 경우 패딩 공간을 활용
#endif
} dtype;

// Global shared variables
dtype 		*sdata;
int 		numthreads;

int main(int argc, char **argv)
{
	int 				i, j;
	pthread_t 			tid[MAXTHREADS];
	int 				myid[MAXTHREADS];

	if (argc != 2) {
		printf("Usage: %s <num_threads> (max threads = 16)\n", argv[0]);
		exit(0);
	}
	numthreads = atoi(argv[1]);
	if (numthreads > MAXTHREADS) {
		printf("numthreads > MAXTHREADS (16)\n");
		exit(0);
	}
	sdata = (dtype *)malloc(sizeof(dtype)*numthreads);  // 공유 공간 할당
	for (i = 0; i < numthreads; i++) {
		(sdata[i]).x = 0;
	}

	// 쓰레드 생성
	for (i = 0; i < numthreads; i++) {
		myid[i] = i;
		pthread_create(&tid[i], NULL, thread_func, (void *)&myid[i]);
	}
	for (i = 0; i < numthreads; i++) {
		pthread_join(tid[i], NULL);
	}


	free(sdata);


#ifdef BAD_FS
	if (numthreads == 1) {
		printf("# Padding: Good   : N=%d : Threads=%d \n", N, numthreads);
		return 0;
	}
#endif
	printf(MSG, N, numthreads);
	return 0;
}


void * thread_func(void *p)
{
	int 		myid = *((int *)p);
	int 		i, j;
	int 		mystart = (myid * (long long)N) / numthreads;
	int 		myend = ((myid + 1) * (long long)N) / numthreads;

	struct {
		struct random_data rdata;
		char rand_state[STATEBUFSZ];	
	} tdata;
	int32_t r;

	initstate_r(1 /* seed */, tdata.rand_state, STATEBUFSZ, &(tdata.rdata));
	for (i = 0; i < REPEAT; ++i) {
		for (j = mystart; j < myend; j++) {
			random_r(&(tdata.rdata), &r);
			sdata[myid].x += j * (r / (RAND_MAX / 2));  // 공유공간에 write
														// GOOD 모드의 경우 패딩으로 인한 거짓 공유 상쇄 효과
		}
	}
}
