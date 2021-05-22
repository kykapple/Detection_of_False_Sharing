/*
 * Capstone Design - Jin Hur, Yeongki Kweon, Jooyoung Kim - Dankook univ, Dept.SW
 * pmatrixcompare.c - A parallel program to compare two matrices element-wise
 */

 //	compile as: gcc -DN=10000 -DGOOD -DCHECK -lpthread -lrt -o <prog>
 //		
 //	run as: ./<prog>  <numthreads>	


#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>  // for perror()

#ifndef 	N
#define 	N 750   	// NxN matrices
#endif
#ifndef		REPEAT
#define		REPEAT 1
#endif
#define		CACHELINE	64				
#define		DATASIZE	4				
#define 	MAXTHREADS 	CACHELINE/DATASIZE	

#ifdef 	GOOD
#define 	MSG 	"# PMatCompare: Good   : N=%d : Threads=%d \n"
#endif
#ifdef 	BAD_MA
#define 	MSG 	"# PMatCompare: Bad-MA : N=%d : Threads=%d \n"
#endif
#ifdef	BAD_FS
#define 	MSG 	"# PMatCompare: Bad-FS : N=%d : Threads=%d \n"
#endif


int 		numthreads;
int 		A[N][N], B[N][N];
int 		pcount[MAXTHREADS];

void* 	compare(void* slice);

int main(int argc, char* argv[])
{
	pthread_t 		tid[MAXTHREADS];
	int 			i, j, k = 0, m, result = 0, count = 0, myid[MAXTHREADS];

	if (argc != 2) {
		printf("Usage: %s number_of_threads\n", argv[0]);
		exit(-1);
	}
	numthreads = atoi(argv[1]);

	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			k = A[i][j] = j % numthreads;
			m = B[i][j] = i % numthreads;
			if (k == m)
				count++;
		}
	}

	for (i = 0; i < numthreads; i++) {
		pcount[i] = 0;
		myid[i] = i;
		if (pthread_create(&tid[i], NULL, compare, &myid[i]) != 0) {
			perror("Can't create thread");
			exit(-1);
		}
	}
	for (i = 0; i < numthreads; i++) 	
		pthread_join(tid[i], NULL);

	for (i = 0; i < numthreads; i++)
		result += pcount[i];

#ifdef BAD_FS
	if (numthreads == 1) {
		printf("# PMatCompare: Good   : N=%d : Threads=%d \n", N, numthreads);
		return 0;
	}
#endif
	printf(MSG, N, numthreads);
	return 0;
}

void* compare(void* slice)					// each thread working on its own slice
{
	int 		myid = *((int *)slice);   		// retrive the slice info
	int 		from = (myid * N) / numthreads; 	// this works even if N indivisible by numthreads
	int 		to = ((myid + 1) * N) / numthreads;
	int 		i, j, k, mycount[MAXTHREADS]; mycount[myid] = 0;

	for (k = 0; k < REPEAT; k++) {
		for (i = from; i < to; i++) {
			for (j = 0; j < N; j++) {
#ifdef GOOD			
				if (A[i][j] == B[i][j])  // 열우선 
					mycount[myid]++;
#elif defined BAD_MA	
				if (A[j][i] == B[j][i])  // 행우선 -> 공간적 지역성 활용x
					mycount[myid]++;
#elif defined BAD_FS	
				if (A[i][j] == B[i][j])
					pcount[myid]++;		 // 거짓 공유 발생
#endif				
			}
		}
	}
#ifndef BAD_FS
	pcount[myid] = mycount[myid];
#endif
}