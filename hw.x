program MY_RPC_PROG {
	version MY_RPC_VERS {
		AB HW(AB) = 1;
	} = 1;
} = 0x30000824;

struct AB {
	int a[5];
	int max;
	int min;
	float avg;
};
