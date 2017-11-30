#include <rpc/rpc.h>
#include "hw.h"



AB *hw_1_svc(AB *ab, struct svc_req *req) {
	puts("*************************************************************");

	struct AB *p;

	printf("getting ready to return value\n");
// Calculating value from Variable.
	ab->max = ab->a[0];
	ab->min=ab->a[0];
	ab->avg=ab->a[0];
	for(int i=1;i<5;i++){
		if (ab->a[i]>ab->max){
			ab->max=ab->a[i];
		}
		else if (ab->a[i]<ab->min){
			ab->min=ab->a[i];
		}
		ab->avg+=ab->a[i];
	}
ab->avg=ab->avg/(float)5;
// print result to the server.
	printf("max=%d\nmin=%d\navg=%f\n",ab->max,ab->min,ab->avg);
	p = ab;
	printf("Returning to Client\n");
	puts("*************************************************************");

	return(p);
}
