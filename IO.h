// IO.h

#define TAM_LINEA_FICHERO	30		// Estimacion del tamano maximo de cada linea
#define TAM_HORA			6		// hh:mm + el fin de cadena
#define NUM_TRIMESTRES		4		// Numero de trimestres al ano
#define BLOQUE				256		// Numero de registros por bloque (es una solucion de compromiso)
									//		Mayor tamano de bloque - menos veces que accedes a memoria para reservar
									//								pero puede quedar mas memoria reservada que no se use

#include <time.h>

struct sRegistro {
	time_t		tiempo;			// Instante de tiempo de un registro
	float		velocidad;		// Velocidad de un registro
	float		produccion;		// Produccion de un registro
};

// NOTA para struct sObservaciones: 
//						Para esta aplicacion que calcula el error por trimestre se almacenan las velocidades y 
//						producciones observadas por trimestre en vectores separados. Esta decision hace que
//						si se quiere calcular otra cosa	como, por ejemplo el error por mes, haya que cambiar esto ya
//						la informacion del mes se ha perdido
//						Se ha disenado asi pensando en no almacenar	datos que no van a ser empleados por la aplicacion
//						como el tiempo de cada registro.
//
//						Si se pensara en hacer mas facilmente extendible la aplicacion podria ser mas logico que la
//						estructura sObservaciones fuera igual a la estructura sPredicciones ya que almacenan el 
//						mismo tipo de datos.
struct sObservaciones {
	unsigned int n[NUM_TRIMESTRES];		// Numero de datos observados por cada trimestre
	float *velocidad[NUM_TRIMESTRES];	// Vector de punteros al comienzo del vector de velocidades observadas de cada trimestre
	float *produccion[NUM_TRIMESTRES];	// Vector de punteros al comienzo del vector de producciones observadas de cada trimestre
};

struct sPredicciones {
	unsigned int n;				 // Numero de datos que hay almacenados en la estructura
	struct sRegistro *registros; // Vector de registros que hay que predecir
};


// Funciones
int cargarDatosObservaciones(char fichero[], struct sObservaciones *o);
int cargarDatosPredicciones(char fichero[], struct sPredicciones *p);
void imprimirPredicciones(const struct sPredicciones *p);
void borrarDatosObservaciones(const struct sObservaciones *o);
void borrarDatosPredicciones(const struct sPredicciones *p);