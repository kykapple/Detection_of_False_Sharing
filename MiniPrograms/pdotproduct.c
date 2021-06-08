/*
 * Capstone Design - Jin Hur, Yeongki Kweon, Jooyoung Kim - Dankook univ, Dept.SW
 * pdotproduct.c - A parallel program to sum the elements of a vector
 */

 //	compile as: gcc -DGOOD -DN=x -DREPEAT=y pdotproduct.c -mcmodel=large -lpthread -lrt -o pdot
 //	use -mcmodel=large with gcc when statically allocating large arrays > 2 GB		
 //	run as: ./pdot <num_threads>


#include <pthread.h>	
#include <stdio.h>
#include <stdlib.h>	// for atoi() etc

#ifndef 	N
#define 	N		10000000 	
#endif

#ifndef 	REPEAT
#define 	REPEAT	1
#endif
#define		CACHELINE	64				
#define		DATASIZE	4			
#define 	MAXTHREADS 	CACHELINE/DATASIZE

#ifdef 	GOOD
#define 	MSG 	"# PDotProd: Good   : N=%d : Threads=%d \n"
#endif
#ifdef 	BAD_MA
#define 	MSG 	"# PDotProd: Bad-MA : N=%d : Threads=%d \n"
#endif
#ifdef	BAD_FS
#define 	MSG 	"# PDotProd: Bad-FS : N=%d : Threads=%d \n"
#endif
#ifndef 	STRIDE
#define	STRIDE	127
#endif

void *sum(void *p);


int 		psum[MAXTHREADS];  
int 		v1[N], v2[N]; 
int	 		sumtotal = 0;
int 		numthreads;


int main(int argc, char **argv)
{
	int 			correct = 0, computed = 0;
	pthread_t 		tid[MAXTHREADS];
	int 			i, myid[MAXTHREADS];

	if (argc != 2) {
		printf("Usage: %s <numthreads>\n", argv[0]);
		exit(0);
	}
	numthreads = atoi(argv[1]);
	if (numthreads > MAXTHREADS) {
		printf("numthreads > MAXTHREADS (%d)\n", MAXTHREADS);
		exit(0);
	}

	for (i = 0; i < N; i++) {
		v1[i] = 1; v2[i] = 2;
	}
	correct = 2 * N;

	for (i = 0; i < numthreads; i++) {
		myid[i] = i;
		psum[i] = 0;
		pthread_create(&tid[i], NULL, sum, &myid[i]);
	}
	for (i = 0; i < numthreads; i++) {
		pthread_join(tid[i], NULL);
	}

	for (i = 0; i < numthreads; i++)
		computed += psum[i];
	correct = REPEAT * correct;

#ifdef BAD_FS
	if (numthreads == 1) {
		printf("# PDotProd: Good   : N=%d : Threads=%d \n", N, numthreads);
		return 0;
	}
#endif
	printf(MSG, N, numthreads);
	return 0;
}

void *sum(void *p)
{
	int 		myid = *((int *)p);
	int 		start = (myid * (long long)N) / numthreads;		
	int 		end = ((myid + 1) * (long long)N) / numthreads;	
	int			i, j, k = myid, n = 1, len;
	int	 		s[MAXTHREADS]; s[myid] = 0;

	for (j = 0; j < REPEAT; j++) {
#ifdef GOOD
		for (i = start; i < end; i++) {
			n += (start + (i * STRIDE + k)) % 2;
			s[myid] += v1[i] * v2[i];	
		}
#elif defined BAD_MA			
		len = end - start;
		for (k = 0; k < STRIDE; k++) {	// bad memory access (strided)
			for (i = 0; i < len / STRIDE; i++) {
				n = start + (i * STRIDE + k);
				s[myid] += v1[n] * v2[n];
			}
		}
		if ((len / STRIDE)*STRIDE != len) {	
			for (n = start + (len / STRIDE)*STRIDE; n < end; n++) { // linearly
				s[myid] += v1[n] * v2[n];			
			}
		}
#elif defined BAD_FS
		for (i = start; i < end; i++) {
			n += (start + (i * STRIDE + k)) % 2;
			psum[myid] += v1[i] * v2[i];	
		}
#endif                         

	}
#ifdef GOOD
	psum[myid] = s[myid];
#endif           
#ifdef BAD_MA
	psum[myid] = s[myid];
#endif 
	s[myid] = n;
	sumtotal += psum[myid];				// lock »ý·«

}
