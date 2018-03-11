#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <crypt.h>
#include <string.h>

int main(int argc, char **argv){
	char * cry;
	char *password=argv[1];
	char * sal= argv[2];
	char salt[150]= "$6$";

	strcat(salt, sal);
	strcat(salt, "$");

	cry= crypt(password,salt);
	printf("%s\n", cry);

	return 0;
}
