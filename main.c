#include <stdio.h>
#include <rpc/rpc.h>
#include "hw.h"
#include <time.h>


main (int argc, char *argv[]) {
	puts("*************************************************************");

	CLIENT *cl;
	struct AB ab;
	struct AB *p;

	if (argc != 2) {
		printf("Usage: client hostname\n");
		exit(1);
	}

	cl = clnt_create(argv[1], MY_RPC_PROG, MY_RPC_VERS, "tcp");
	if (cl == NULL) {
		clnt_pcreateerror(argv[1]);
		exit(1);
	}
//randomly generate number
srand(time(NULL));
	ab.a[0] = rand()%100;
	ab.a[1] = rand()%100;
	ab.a[2]=rand()%100;
	ab.a[3]=rand()%100;
	ab.a[4]=rand()%100;
	printf("Getting ready to call Server method,\n The random values are:\n%d\n%d\n%d\n%d\n%d\n",ab.a[0],ab.a[1],ab.a[2],ab.a[3],ab.a[4]);
	p = hw_1(&ab, cl);
//Returning from Server.
	printf("Back from Server\n");
	if (p == NULL) {
		clnt_perror(cl,argv[1]);
		exit(1);
	}

	printf("Returned min=%d\n, max=%d\n, avg=%f\n",p->min,p->max,p->avg);
	puts("*************************************************************");

	return 0;
}
