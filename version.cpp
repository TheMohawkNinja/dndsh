#include <stdio.h>

int main()
{
	const unsigned char major = 0;
	const unsigned char minor = 3;
	const unsigned char patch = 2;
	fprintf(stdout,"Version %d.%d.%d\n",major,minor,patch);

	return 0;
}
