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
	int qACKS; 
	key_t clave;

	clave = ftok(FTOK_DIR, Q_ACKS);
	qACKS = msgget(clave,0660);
	msgctl(qACKS, IPC_RMID, NULL);
	
	return 0;
}

