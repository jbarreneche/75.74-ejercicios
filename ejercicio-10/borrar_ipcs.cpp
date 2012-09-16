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
#include <sys/msg.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	int shmControl; 
	int semSet;	
	key_t clave;

	clave = ftok(FTOK_DIR, SHM_CONTROL);
	shmControl = shmget (clave, sizeof(CONTROL), 0660);
	shmctl(shmControl,IPC_RMID,(struct shmid_ds *)  0);

	semSet = getsem (SEM_SET);
	elisem(semSet);
	
	return 0;
}

