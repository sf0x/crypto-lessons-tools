#include <gmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define HELP "Usage: ./bsgs <a> <a> <p>\n"




void stop(short v){
	printf("%s", HELP);
	switch(v){
		case 0: printf("Error: <a> is missing!\n"); break;
		case 1: printf("Error: <b> is missing!\n"); break;
		case 2: printf("Error: <p> is missing!\n"); break;
		default: break; 
	}
	EXIT_FAILURE;
}

int main(int argc, char *argv[]){
	for(uint8_t i=0; i<3; i++){
		if(!argv[i])
			stop(i);
	}
	
	mpz_t a,
		  b,
		  p, 
		  sqrm;

	mpz_init(a);
	mpz_init(b);
	mpz_init(p);
	mpz_init(sqrm);

	mpz_set_ui(a,atoi(argv[0]));
	mpz_set_ui(b,atoi(argv[1]));
	mpz_set_ui(p,atoi(argv[2]));
	mpz_set_ui(sqrm,lrint(sqrt(p-1)));




	mpz_clear(a);
	mpz_clear(b);
	mpz_clear(p);
	return 0;
}