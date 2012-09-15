/*
 * Programa: TcpBorrarIPC
 *  
 *  Created by Maria Feldgen on 3/10/12.
 *  Copyright 2012 Facultad de Ingenieria U.B.A. All rights reserved.
 *
 * Borra los IPCs del servidor para terminar los programas.
 */
#include	"TcpEjemploAplicacion.h"
#include	"TcpSocketsGral.h"
#include	"semaforos.h"

int main(int argc, char *argv[]) {
	int shmid, compras1, compras2, despacho1, despacho2;
	char mostrar[120];	/* mensaje para mostrar en pantalla */
	char *pname;		/* nombre del programa */
	int pid_pr;
	int mutex;			/* file descriptor de los IPCs */
	key_t clave;
	STOCK a;
	
	clave = ftok(DIRECTORIO,SHM);
	shmid = shmget (clave, sizeof(a), 0660);
	shmctl(shmid,IPC_RMID,(struct shmid_ds *)  0);

	mutex = getsem (MUTEX);
	elisem(mutex);
	
	clave = ftok(DIRECTORIO,COLASR);
	compras1 = msgget (clave,0660);
	msgctl(compras1, IPC_RMID, NULL);
	
	clave = ftok(DIRECTORIO,COLASE);
	despacho1 = msgget (clave,0660);
	msgctl(despacho1, IPC_RMID, NULL);
	
	clave = ftok(DIRECTORIO,COLACR);
	compras2 = msgget (clave,0660);
	msgctl(compras2, IPC_RMID, NULL);
	
	clave = ftok(DIRECTORIO,COLACE);
	despacho2 = msgget (clave,0660);
	msgctl(despacho2, IPC_RMID, NULL);
	exit(0);
}

