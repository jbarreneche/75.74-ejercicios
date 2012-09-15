#include "constantes.h"
#include "lib/semaphore.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
	char mostrar[120];	/* mensaje para mostrar en pantalla */
	char *pname;		/* nombre del programa */
	int pid_pr;
	key_t clave;

	int salemanNumber;

	pname = argv[0];
	pid_pr =getpid();

	if (!argc) {
		sprintf (mostrar, "Missing arguments"); 
		write(fileno(stdout), mostrar, strlen(mostrar));
		exit(1);
	}

	salemanNumber = atoi(argv[1]);
	sprintf (mostrar, "Launched vendedor %d\n", salemanNumber); 
	write(fileno(stdout), mostrar, strlen(mostrar));

	return 0;
}