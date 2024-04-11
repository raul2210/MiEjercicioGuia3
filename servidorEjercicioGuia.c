#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <pthread.h>
int cont;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void *AntenderCliente(void *socket)
{
	int sock_conn;
	int *s;
	s = (int * ) socket;
	sock_conn=*s;
	
	int ret;
	char peticion[512];
	char respuesta[512];
	//sock_conn es el socket que usaremos para este cliente
	//bucle de atencion al cliente
	int terminar = 0;
	while (terminar ==0)
	{
		// Ahora recibimos su peticion
		ret=read(sock_conn,peticion, sizeof(peticion));
		printf ("Recibida una petición\n");
		// Tenemos que a?adirle la marca de fin de string 
		// para que no escriba lo que hay despues en el buffer
		peticion[ret]='\0';
		
		//Escribimos la peticion en la consola
		
		printf ("La petición es: %s\n",peticion);
		
		char *p = strtok(peticion, "/");
		int codigo =  atoi (p);
		
		if (codigo == 0)
			terminar = 1;
		else if (codigo == 4)
			sprintf (respuesta,"%d\n",cont);
		else if (codigo ==1) //piden la longitd del nombre
		{
			p = strtok( NULL, "/");
			char nombre[20];
			strcpy (nombre, p);
			sprintf (respuesta,"%d\n",strlen(nombre));
		}
		else if (codigo == 2)
		{
			p = strtok( NULL, "/");
			char nombre[20];
			strcpy (nombre, p);
			// quieren saber si el nombre es bonito
			if((nombre[0]=='M') || (nombre[0]=='S'))
				strcpy (respuesta,"SI");
			else
				strcpy (respuesta,"NO");
		}
		else //es alto?
		{
			p = strtok( NULL, "/");
			char nombre[20];
			strcpy (nombre, p),
				
				p = strtok( NULL, "/");
			float altura= atof(p);
			
			
			if(altura >=1.80)
				sprintf(respuesta,"%s eres alto",nombre);
			else if(altura >= 1.60)
				sprintf(respuesta,"%s no eres tan alto pero esta bien igual :3",nombre);
			else
				sprintf(respuesta,"%s eres MUY bajo", nombre);
			// Enviamos la respuesta
		}
		if (codigo != 0)
		{
			printf("respuesta:%s\n",respuesta);
			write (sock_conn,respuesta, strlen(respuesta));
		}
		if((codigo == 1)||(codigo == 2)||(codigo == 3))
		{
			phtread_mutex_lock(&mutex);
			cont++;
			phtread_mutex_unlock(&mutex);
		}
	}
	// Se acabo el servicio para este cliente
	close(sock_conn); 
}
int main(int argc, char *argv[])
{
	
	cont = 0;
	int sock_conn, sock_listen;
	struct sockaddr_in serv_adr;

	// INICIALITZACIONS
	// Obrim el socket
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("Error creant socket");
	// Fem el bind al port
	memset(&serv_adr, 0, sizeof(serv_adr));// inicialitza a zero serv_addr
	serv_adr.sin_family = AF_INET;
	// asocia el socket a cualquiera de las IP de la m?quina. 
	//htonl formatea el numero que recibe al formato necesario
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	// escucharemos en el port 9050
	serv_adr.sin_port = htons(9100);
	if (bind(sock_listen, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) < 0)
		printf ("Error al bind");
	//La cola de peticiones pendientes no podr? ser superior a 4
	if (listen(sock_listen, 4) < 0)
		printf("Error en el Listen");
	int i;
	// Atenderemos solo 10 peticione
	int sockets[100];
	pthread_t thread;
	for(i=0;i<10;i++){
		printf ("Escuchando\n");
		
		sock_conn = accept(sock_listen, NULL, NULL);
		printf ("He recibido conexion\n");
		sockets[i] = sock_conn;
		pthread_create (&thread,NULL,AntenderCliente,&sockets[i]);

	}
}
