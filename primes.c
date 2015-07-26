#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <gmp.h>
#define THREADS 10
#define IS_PRIME 0
#define IS_COMPOSITE 1
#define EXIT_SUCCESS 0
#define EXIT_ERROR 1
struct stateStruct {
	unsigned long p_exp;
	unsigned long twos;
	unsigned long threes;
	unsigned long sevens;
};
int initState(struct stateStruct *);
int nextMersennePrime(struct stateStruct *, unsigned long *);
int master(struct stateStruct *);
void *worker(void *);
int main(int argc, char **argv) {
	struct stateStruct state;
	printf("Mersenne Prime Program (C)Copyright 2015 Manny Peterson All Rights Reserved.\n");
	if (initState(&state) == EXIT_ERROR) {
		exit(EXIT_ERROR);
	}
	for (;;) {
		if (master(&state) == EXIT_ERROR) {
			exit(EXIT_ERROR);
		}
	}
	exit(EXIT_SUCCESS);
}
int initState(struct stateStruct *state) {
	if (state == NULL) {
		return(EXIT_ERROR);
	}
	state->p_exp = 2;
	state->twos = 4;
	state->threes = 6;
	state->sevens = 14;
	return(EXIT_SUCCESS);
}
int nextMersennePrime(struct stateStruct *state, unsigned long *p_exp) {
	int num;
	if (state == NULL) {
		return(EXIT_ERROR);
	}
	if (p_exp == NULL) {
		return(EXIT_ERROR);
	}
	for (;;) {
		num = IS_PRIME;
		if (state->p_exp == state->twos) {
			num = IS_COMPOSITE;
			state->twos += 2UL;
		}
		if (state->p_exp == state->threes) {
			num = IS_COMPOSITE;
			state->threes += 3UL;
		}
		if (state->p_exp == state->sevens) {
			num = IS_COMPOSITE;
			state->sevens += 7UL;
		}
		memcpy(p_exp, &state->p_exp, sizeof(unsigned long));
		state->p_exp += 1UL;
		if (num == IS_PRIME) {
			break;
		} 
	}
	return(EXIT_SUCCESS);
}
int master(struct stateStruct *state) {
	int i;
	pthread_t thread[THREADS];
	unsigned long p_exp[THREADS];
	if (state == NULL) {
		return(EXIT_ERROR);
	}
	for (i = 0;i < THREADS;i++) {
		if (nextMersennePrime(state, &p_exp[i]) == EXIT_ERROR) {
			return(EXIT_ERROR);
		}
		if (pthread_create(&thread[i], NULL, worker, (void *) &p_exp[i])) {
			printf("error: unable to create thread.\n");
			return(EXIT_ERROR);
		}
	}
	for (i = 0;i < THREADS;i++) {
		if (pthread_join(thread[i], NULL)) {
			printf("error: unable to join thread.\n");
			return(EXIT_ERROR);
		}
	}
	return(EXIT_SUCCESS);
}
void *worker(void *ptr) {
	mpz_t c;
	int res;
	unsigned long p_exp;
	if (ptr == NULL) {
		pthread_exit(NULL);
	}
	memcpy(&p_exp, ptr, sizeof(unsigned long));
	mpz_init(c);
	mpz_ui_pow_ui(c, 2UL, p_exp);
	mpz_sub_ui(c, c, 1UL);
	res = mpz_probab_prime_p(c, 25);
	if (res == 2) {
		printf("2^%lu-1 is PRIME\n", p_exp);
	} else if (res == 1) {
		printf("2^%lu-1 is PROBABLE\n", p_exp);
	}	
	pthread_exit(NULL);
}
