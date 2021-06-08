/*
 * Capstone Design - Jin Hur, Yeongki Kweon, Jooyoung Kim - Dankook univ, Dept.SW
 * psumscalar.c - A simple parallel sum program to sum a series of scalars
 */

 //	compile as: gcc -DGOOD -DN=x -DREPEAT=y psumscalar.c -lpthread -lrt -o psums
 //			
 //	run as: ./psums <num_threads>


#include <pthread.h>	
#include <stdio.h>
#include <stdlib.h>	// for atoi() etc

#ifndef 	N
#define 	N		300000000 	
#endif				

#ifndef 	REPEAT
#define 	REPEAT	1
#endif
#define		CACHELINE	64				// ĳ�� ���� ũ��: 64bytes
#define		DATASIZE	4				// int=4 bytes
#define 	MAXTHREADS 	CACHELINE/DATASIZE	// ��� ������ ���� ���� ���� ������ ����

#ifdef		GOOD
#define 	MSG 	"# PSumScalar: Good   : N=%d : Threads=%d \n"
#endif
#ifdef		BAD_FS
#define 	MSG 	"# PSumScalar: Bad-FS : N=%d : Threads=%d \n"
#endif

void *sum(void *p);

// Global shared variables
int 		psum[MAXTHREADS];  // partial sum computed by each thread
int	 		sumtotal = 0;
int 		numthreads;

int main(int argc, char **argv)
{
	int 			result = 0;
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
;
	// ������ ����
	for (i = 0; i < numthreads; i++) {
		myid[i] = i;
		psum[i] = 0;
		pthread_create(&tid[i], NULL, sum, &myid[i]);
	}
	for (i = 0; i < numthreads; i++) {
		pthread_join(tid[i], NULL);
	}


	for (i = 0; i < numthreads; i++)
		result += psum[i];


#ifdef BAD_FS
	if (numthreads == 1) {
		printf("# PSumScalar: Good   : N=%d : Threads=%d \n", N, numthreads);
		return 0;
	}
#endif
	printf(MSG, N, numthreads);

	return 0;
}
//-----------------------------------------------
void *sum(void *p)
{
	int 		myid = *((int *)p);
	long long 	start = (myid * (long long)N) / numthreads;
	long long	end = ((myid + 1) * (long long)N) / numthreads;
	int		i, j;
	int		s[MAXTHREADS]; s[myid] = 0;  // ��������(�迭�� ���������ν� �������� ����)

	for (j = 0; j < REPEAT; j++) {
		for (i = start; i < end; i++) {
#ifdef GOOD
			s[myid] += i % 3;			 // �������� ���. => ���� ���� ����
#elif defined BAD_FS        
			psum[myid] += i % 3;		 // �����尣 ���� ���� �߻�
#endif                         
		}
	}
#ifdef GOOD	                                   
	psum[myid] = s[myid];
#endif           
	sumtotal += psum[myid];				// Lock ����.       
}