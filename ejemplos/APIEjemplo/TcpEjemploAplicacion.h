/*
 *  TcpEjemploAplicacion.h
 *  
 *
 *  Created by Maria Feldgen on 3/10/12.
 *  Copyright 2012 Facultad de Ingenieria U.B.A. All rights reserved.
 *   
 * Definicion de tipos de estructuras y constantes de la Aplicacion de Venta de productos.
 */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/shm.h>
#include <errno.h>
#include <string.h>
#include <sys/msg.h>

#define TRUE 1
#define FALSE 0
#define DIRECTORIO "/home/knoppix/Distribuidos/ejemplos"

#define PRODUCTOS 40
#define VENDEDORES 5
#define CLIENTES 6

#define PEDI 2
#define ACEP 3
#define PROD 4
#define CONF 5
#define FACT 6
#define FINV 9

typedef struct      {	/* Producto a vender */
	int codigo;			/* Codigo */
	char denominacion[22]; /* Denominacion */
	float precio;
	int cantidad;		/* stock o cantidad vendida */
} PRODUCTO;

typedef struct {		/* stock de productos */
	int cantProductos;	/* Cantidad de productos a vender */
	PRODUCTO prod[25];  /* tabla de productos */
} STOCK;

typedef struct      {   /* datos del cliente */
	char razonSocial[25];  /* razon social del cliente */
	char cuit[15];		/* nro de CUIT del cliente */
	char tipo;			/* tipo de factura */
} CLIENTE;

typedef struct      {   /* datos de la compra */
	int producto;		/* codigo del producto a comprar */
	int cantidad;		/* cantidad de unidades */
	int mas;			/* 1 indica que hay mas productos */
} COMPRA;

typedef struct      {   /* Confirmacion si hay en stock */
	int producto;		/* codigo del producto a comprar */
	int cantidad;		/* cantidad de unidades */
} CONFIRMACION;

typedef struct      {   /* Factura */
	char empresa[16];	/* empresa  del vendedor */
	CLIENTE clien;		/* cliente que compra */
	int cantProductos;	/* cantidad de productos comprados*/
	PRODUCTO prod[10];	/* productos vendidos */
	float total;		/* valor total */
	float iva;
	float neto;
} FACTURA;

	


