/*
 * Capstone Design - Jin Hur, Yeongki Kweon, Jooyoung Kim - Dankook univ, Dept.SW 
 * psumvector.c - A parallel program to sum the elements of a vector
 */

 //	compile as: gcc -DGOOD -DN=x -DREPEAT=y psumvector.c -lpthread -lrt -o psumv
 //			
 //	run as: ./psumv <num_threads>


#include <pthread.h>	
#include <stdio.h>
#include <stdlib.h>	// for atoi() etc

#ifndef 	N
#define 	N		100000000 	
#endif			

#ifndef 	REPEAT
#define 	REPEAT	1
#endif
#define		CACHELINE	64					// ĳ�� ���� ũ��: 64bytes
#define		DATASIZE	4					// int=4 bytes
#define 	MAXTHREADS 	CACHELINE/DATASIZE	// ��� ������ ���� ���� ���� ������ ����


#ifdef 	GOOD
#define 	MSG 	"# PSumVector: Good   : N=%d : Threads=%d \n"
#endif
#ifdef 	BAD_MA
#define 	MSG 	"# PSumVector: Bad-MA : N=%d : Threads=%d \n"
#endif
#ifdef	BAD_FS
#define 	MSG 	"# PSumVector: Bad-FS : N=%d : Threads=%d \n"
#endif
#ifndef 	STRIDE
#define		STRIDE	127
#endif

void *sum(void *p);

int	 		psum[MAXTHREADS];		// �� ������� �κ���(partitial sum)�� ���Ѵ�.
int 		vector[N];				// for check 
int 		sumtotal = 0;
int 		numthreads;

int main(int argc, char **argv)
{
	int 			correctsum = 0, result = 0;
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
		vector[i] = i % 3;
		correctsum += vector[i];
	}

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
		printf("# PSumVector: Good   : N=%d : Threads=%d \n", N, numthreads);
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
			n += (start + (i * STRIDE + k)) % 2;  // <= BAD_MA ��� ������ ���ϸ� �ֱ�����
			s[myid] += vector[i];				  // �������� ���, ���� ���� ȸ��
		}
#elif defined BAD_MA			
		len = end - start;
		for (k = 0; k < STRIDE; k++) {		   // ��Ʈ���̵�� ���� Bad Memory access (������ ������ Ȱ�� ����)
			for (i = 0; i < len / STRIDE; i++) {
				n = start + (i * STRIDE + k);  // Large foot print
				s[myid] += vector[n];		   // �������� ���, ���� ���� ȸ��
			}
		}
		if ((len / STRIDE)*STRIDE != len) {	   // ���� ��� ó��
			for (n = start + (len / STRIDE)*STRIDE; n < end; n++) { // linearly
				s[myid] += vector[n];   				
			}
		}
#elif defined BAD_FS
		for (i = start; i < end; i++) {
			n += (start + (i * STRIDE + k)) % 2;  // <= BAD_MA ��� ������ ���ϸ� �ֱ�����
			psum[myid] += vector[i];			  // ���� �迭 ��ҿ� ���� �����Ͽ� ���� ���� �߻�
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
	sumtotal += psum[myid];				// lock ����
}
