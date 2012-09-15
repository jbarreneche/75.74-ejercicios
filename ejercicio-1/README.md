# Ejercicio Nº 1: 

El uso clásico de un semáforo binario es que solo un proceso por vez
pueda acceder al recurso. Supongamos que tenemos una variable compartida que es la
cantidad de tickets que faltan vender y queremos coordinar el acceso por múltiples procesos.
En este caso es un problema de exclusión mutua, el semáforo asegura que solo un proceso
acceda y modifique a la variable en cada momento. 

Un proceso inicial crea una cierta cantidad de procesos vendedores (> 2)
que venden los tickets y un conjunto de procesos clientes (> 2) que compran los tickets. Los clientes pueden comprar entre 1 y 4 tickets, de a un ticket por vez. El cliente
envía un monto de dinero al vendedor, que verifica que el monto sea mayor o igual que el
precio del ticket. 

Si el dinero es mayor o igual al precio del ticket y hay tickets
disponibles, el vendedor envía al cliente el Nro de ticket vendido, el vuelto y el aviso que la
compra fue exitosa. Si no es suficiente el monto, devuelve un aviso de “monto insuficiente” y
si no hay mas tickets, devuelve “no quedan mas tickets”.

Cada cliente extrae los datos de la compra de un archivo, para simular
las distintas situaciones. 

Se pide:
1. Haga un diagrama de secuencia incluyendo los IPC como objetos del problema planteado. 
2. Escriba un programa inicial para inicializar los IPCs y para lanzar los procesos, tal como se explicó en clase. 
3. Escriba un programa final para destruir los IPCs y parar los procesos vendedor y cliente (si hay alguno pendiente). 
4. Escriba los programas del problema y agregue un makefile para compilarlos.
5. Si los datos para el intercambio en el problema son números use secuencias de números, si son de varios tipos, use archivos distintos para cada proceso. 
6. Simule el tiempo de procesamiento con un sleep (usleep) con tiempo variable (use un generador de números al azar).
7. Las corridas deben ser significativas. No haga pruebas con solamente uno o dos valores. Ud. debe asegurar que no hay un busy wait, ni starvation (innación) ni un deadlock.

Nota: Verifique que cada vez que lo corre, la salida por pantalla será
diferente porque los procesos no se planifican exactamente de la misma forma.
