/*
 * Capstone Design - Jin Hur, Yeongki Kweon, Jooyoung Kim - Dankook univ, Dept.SW
 * counting.c - A simple counting parallel program to demo false sharing
 */

 //	compile as: gcc -DN=<array_size> -DREPEAT=<n> -D[GOOD|BAD] counting.c -lpthread -lrt -o count
 //		
 //	run as: ./count <numthreads>	

#include <pthread.h>	
#include <stdio.h>
#include <stdlib.h>	// for atoi() etc

#ifndef 	N
#define 	N		100000000
#endif

#ifndef 	REPEAT
#define 	REPEAT	1	// to repeat iterations (for better data collection)
#endif
#define		CACHELINE	64				// size of cacheline is 64 bytes
#define		DATASIZE	4				// int=4 bytes, long long is 8 bytes
#define 	MAXTHREADS 	CACHELINE/DATASIZE	// max # parallel threads to sum (with false sharing)	

#ifdef 	GOOD
#define 	MSG 	"# PCount: Good   : N=%d \n"
#endif
#ifdef 	BAD_MA
#define 	MSG 	"# PCount: Bad-MA : N=%d : Threads=%d \n"
#endif
#ifdef	BAD_FS
#define 	MSG 	"# PCount: Bad-FS : N=%d : Threads=%d \n"
#endif
#ifndef 	STRIDE
#define	STRIDE	127
#endif

void *pcount(void *vargp);

int count[MAXTHREADS];
int x[N];
int totalcount = 0, numthreads;

int main(int argc, char **argv)
{
	int 			i, result = 0;
	pthread_t 		main_tid, tid[MAXTHREADS];
	int 			myid[MAXTHREADS], status;

	if (argc != 2) {
		printf("Usage: %s <num_threads> (max threads = 16)\n", argv[0]);
		exit(0);
	}
	numthreads = atoi(argv[1]);
	if (numthreads > MAXTHREADS) {
		printf("nthreads > MAXTHREADS (16)\n");
		exit(0);
	}
	for (i = 0; i < N; i++)
		x[i] = random();	

	for (i = 0; i < numthreads; i++) {
		myid[i] = i;
		count[i] = 0;
		pthread_create(&tid[i], NULL, pcount, &myid[i]);
	}
	for (i = 0; i < numthreads; i++) {
		pthread_join(tid[i], NULL);
	}

	for (i = 0; i < numthreads; i++)
		result += count[i];

#ifdef BAD_FS
	if (numthreads == 1) {
		printf("# PCount: Good   : N=%d : Threads=%d \n", N, numthreads);
		return 0;
	}
#endif
	printf(MSG, N, numthreads);
	return 0;
}

void * pcount(void *p)
{
	int 		myid = *((int *)p);
	int 		start = (myid * (long long)N) / numthreads;
	int 		end = ((myid + 1) * (long long)N) / numthreads;
	int			i, j, k = myid, n = 1, len;
	int	 		mycount[MAXTHREADS]; mycount[myid] = 0;

	for (j = 0; j < REPEAT; ++j) {
#ifdef GOOD
		for (i = start; i < end; i++) {
			n += (start + (i * STRIDE + k)) % 2;
			if (x[i] % 2 == 0)
				mycount[myid]++;
		}
#elif defined BAD_MA			
		len = end - start;
		for (k = 0; k < STRIDE; k++) {	// bad memory access (strided)
			for (i = 0; i < len / STRIDE; i++) {
				n = start + (i * STRIDE + k);
				if (x[n] % 2 == 0)
					mycount[myid]++;
			}
		}
		if ((len / STRIDE)*STRIDE != len) {							// remainder, if any 
			for (n = start + (len / STRIDE)*STRIDE; n < end; n++) { // linearly
				if (x[n] % 2 == 0)
					mycount[myid]++;
			}
		}
#elif defined BAD_FS
		for (i = start; i < end; i++) {
			n += (start + (i * STRIDE + k)) % 2;
			if (x[i] % 2 == 0)
				count[myid]++;	
		}
#endif                         
	}
#ifdef GOOD
	count[myid] = mycount[myid];
#endif           
#ifdef BAD_MA
	count[myid] = mycount[myid];
#endif
	mycount[myid] = n;
	totalcount += count[myid];		// lock »ý·«
}
