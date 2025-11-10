#include "includes/lem-ipc.h"

int main()
{
	srand(time(NULL) ^ getpid());
	printf("nombre alea: %d\n", (rand() % 3) - 1);
}