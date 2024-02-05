#include <stdio.h>

int main(int argc, char * argv[])
{
	int i;
	FILE * fp = fopen("c:\\git\\tempargs", "w");
	if (!fp)
	{
		printf("temprory args file can not open...\n");
		return 0;
	}
	for(i = 0; i < argc; i++)
	{
		fprintf(fp, "%s\n", argv[i]);	
	}	
	fclose(fp);
	return argc;
}
