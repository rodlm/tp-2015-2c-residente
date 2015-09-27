// Bibliotecas //
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/log.h>
#include <semaphore.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <netinet/in.h>
#include <resolv.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <arpa/inet.h>


#define COLOR_VERDE   			"\x1b[32m"
#define DEFAULT   				"\x1b[0m"
#define PATH_CONFIG 			"config.cfg"		//Ruta del config
#define NOMBRE_ARCHIVO_LOG 		"memoria.log"			//Nombre de archivo de log		//Cantidad maxima de directorios
#define TAMANIO_IP				16					//un string ejempl 192.168.001.123


/*********************/
t_log* logger;								// Logger del commons
char* g_MensajeError;						//Mensaje de error global.
pthread_t hConsola;	// Definimos los hilos principales

char* g_Ip_Memoria;
int g_Puerto_Memoria;
char* g_Ip_Swap;
char* g_Puerto_Swap;
int g_Max_Marcos_Proc;
int g_Cant_Marcos;
int g_Tam_Marcos;
int g_Entradas_Tlb;
char* g_Tlb_Habilitada;
int g_Retardo_Memoria;
int g_Ejecutando = 1;						// - Bandera que controla la ejecución o no del programa. Si está en 0 el programa se cierra.
t_list* lista_cpu; 							//Lista de Cpus conectadas.
#define BUFFERSIZE 200
pthread_t hOrquestadorConexiones;

typedef struct {
	char * ip;
	char * puerto;
	int procesoActivo;
} t_cpu;

t_cpu *cpu_create(char *ipCpu, char* puertoCpu) {
	t_cpu *new = malloc(sizeof(t_cpu));
	new->ip = strdup(ipCpu);
	new->puerto = strdup(puertoCpu);
	new->procesoActivo = 0;
	return new;
}

void cpu_destroy(t_cpu* self) {
	free(self);
}


// TIPOS //
typedef enum {
	CantidadArgumentosIncorrecta,
	NoSePudoAbrirConfig,
	NoSePuedeObtenerPuerto,
	NoSePuedeObtenerNodos,
	NosePuedeCrearHilo,
	OtroError,
} t_error;							//Tipo error


typedef struct {

} t_tlb;

int conectarConSwap(int *socket_swap);
void ConectarseConSwap(int g_puerto_memoria);
void LevantarConfig();
void Error(const char* mensaje, ...);
char* RecibirDatos(int socket, char *buffer, int *bytesRecibidos,int *cantRafaga,int *tamanio);
int AtiendeCliente(void * arg);
void HiloOrquestadorDeConexiones();
int EnviarDatos(int socket, char *buffer, int cantidadDeBytesAEnviar);
void CerrarSocket(int socket);
void HiloOrquestadorDeConexiones();
char* DigitosNombreArchivo(char *buffer, int *posicion);
void informarConexionCPU(char* buffer);
void informarFinDelProceso(char* buffer);
void informarInicio(char* buffer);
void informarLeer(char* buffer);
void informarEscribir(char* buffer);
char* obtenerSubBuffer(char *nombre);
void inicioProcesoSwap(int pid, int cant_pag);
void finProcesoSwap(int pid);
void leerSwap(int pid, int num_pag);
void escribirSwap(int pid, int num_pag, char* contenido);
void resultadoInicioSwap(char* buffer);
void resultadoLecturaSwap(char* buffer);
void resultadoEscrituraSwap(char* buffer);
void resultadoFinSwap(char* buffer);
int conectarConCpu(int *socket_cpu, char*ip, char*puerto);
t_cpu* buscarCPUporPid(int pid);
t_cpu* buscarCPUporPuerto(char* puerto);
int finProcesoCpu(char*ip, char*puerto);
int inicioProcesoCpu(char*ip, char*puerto,char* resultado);
int escribirCpu(char*ip, char*puerto,char* resultado);
int leerCpu(char*ip, char*puerto,char* pagina,char* contenido);

