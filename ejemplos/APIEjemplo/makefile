# Ejemplo del uso de una API para una aplicacion de compra de productos en ambiente Cliente/Servidor

MYLIB= TcpPrimitivas/TcpEnviar.o TcpPrimitivas/TcpRecibir.o TcpPrimitivas/TcpOpenActivo.o TcpPrimitivas/TcpOpenPasivo.o
LIBS	=
CFLAGS	= -O -g

# Internet stream version (TCP protocol).
#

tcp:	 TcpServerConcurrente TcpEjemploComprador TcpEjemploVendedor TcpComEntrantes TcpComSalientes TcpBorrarIPC

 TcpServerConcurrente.o TcpEjemploComprador.o TcpEjemploVendedor.o TcpComEntrantes.o TcpComSalientes.o TcpBorrarIPC.o: TcpEjemploAplicacion.h TcpEjemploClienteAPI.h TcpEjemploServerAPI.h

TcpServerConcurrente:	TcpServerConcurrente.o $(MYLIB)
		c++ $(CFLAGS) -o $@ TcpServerConcurrente.o  $(MYLIB) $(LIBS)

TcpEjemploComprador:	TcpEjemploComprador.o  $(MYLIB)
		c++ $(CFLAGS) -o $@ TcpEjemploComprador.o  $(MYLIB) $(LIBS)

TcpEjemploVendedor:	TcpEjemploVendedor.o  $(MYLIB)
		c++ $(CFLAGS) -o $@ TcpEjemploVendedor.o  $(MYLIB) $(LIBS)

TcpComEntrantes: TcpComEntrantes.o  $(MYLIB)
		c++ $(CFLAGS) -o $@ TcpComEntrantes.o  $(MYLIB) $(LIBS)

TcpComSalientes: TcpComSalientes.o  $(MYLIB)
		c++ $(CFLAGS) -o $@ TcpComSalientes.o  $(MYLIB) $(LIBS)
		
TcpBorrarIPC: TcpBorrarIPC.o  $(MYLIB)
		c++ $(CFLAGS) -o $@ TcpBorrarIPC.o  $(MYLIB) $(LIBS)
clean:
	rm -f *.o TcpServerConcurrente TcpEjemploComprador TcpEjemploVendedor TcpComEntrantes TcpComSalientes TcpBorrarIPC
