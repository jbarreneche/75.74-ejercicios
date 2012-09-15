/*
 * Programa: TcpBorrarIPC
 *  
 *  Created by Maria Feldgen on 3/10/12.
 *  Copyright 2012 Facultad de Ingenieria U.B.A. All rights reserved.
 *
 * Borra los IPCs del servidor para terminar los programas.
 */
#include "constantes.h"
#include "lib/semaphore.h"
#include <sys/shm.h>

int main(int argc, char *argv[]) {
	int shmid; //, compras1, compras2, despacho1, despacho2;
	char mostrar[120];	/* mensaje para mostrar en pantalla */
	char *pname;		/* nombre del programa */
	int pid_pr;
	int mutex;			/* file descriptor de los IPCs */
	key_t clave;

	clave = ftok(FTOK_DIR,SHM);
	shmid = shmget (clave, sizeof(TICKETS), 0660);
	shmctl(shmid,IPC_RMID,(struct shmid_ds *)  0);

	mutex = getsem (MUTEX);
	elisem(mutex);
	
	// clave = ftok(FTOK_DIR,COLASR);
	// compras1 = msgget (clave,0660);
	// msgctl(compras1, IPC_RMID, NULL);
	
	// clave = ftok(FTOK_DIR,COLASE);
	// despacho1 = msgget (clave,0660);
	// msgctl(despacho1, IPC_RMID, NULL);
	
	// clave = ftok(FTOK_DIR,COLACR);
	// compras2 = msgget (clave,0660);
	// msgctl(compras2, IPC_RMID, NULL);
	
	// clave = ftok(FTOK_DIR,COLACE);
	// despacho2 = msgget (clave,0660);
	// msgctl(despacho2, IPC_RMID, NULL);
	return 0;
}

