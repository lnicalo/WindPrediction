#include "IO.h"

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <time.h>

/*
 Funcion:	cargarDatosObservaciones
 Tarea:		Lee las observaciones de un fichero de texto y los almacena en memoria
 Parametros:
			fichero:	entrada		puntero al inicio de la cadena del nombre del archivo
			datos:		salida		puntero a la estructura donde se almacenan los datos
*/

int cargarDatosObservaciones(char fichero[], struct sObservaciones *o) {
	FILE *fp;						// Puntero 
	char linea[TAM_LINEA_FICHERO];	// cadena de donde se almacena las lineas que se leen del archivo

	unsigned int ano;				// Ano del registro leido
	unsigned int mes;				// Mes del registro leido
	unsigned int dia;				// Dia del registro leido
	unsigned int trimestre;			// Trimestre al que pertenece el mes del registro leido.
									//		Toma valores: 0,1,2,3.
	unsigned int hora;				// Hora del registro leido
	unsigned int minuto;			// Minutos del registro leido
	float vel;						// Velocidad del viento del registro leido
	float prod;						// Produccion del registro leido

	unsigned int i;					// Variable de control para bucles
	unsigned int num_bloques[NUM_TRIMESTRES]; // Numero de bloques reservados por cada una de la separaciones
	
	// Abrimos el fichero
	if ((fp = fopen(fichero,"r")) == NULL) {
		return -1;
	}

	// Buscamos la linea que tiene la palabra "observaciones" ya que La siguiente linea
	// a aquella que tiene la palabra observaciones contiene la primera linea de datos
	do{ 
		if( fgets(linea,TAM_LINEA_FICHERO,fp) == NULL ) {
			// Se produjo un error o se llego a EOF sin encontrar la palabra
			return -1;
		}

		// fgets tambien pone el caracter de nueva linea. Lo eliminamos
		linea[strlen(linea)-1] = 0;
	} while(strcmp(linea,"observaciones") != 0);

	// Inicializamos los datos:
	//		Numero de bloques a 0
	//		Numero de elementos a 0 
	//		Los punteros al inicio de los vectores de las velocidades 
	//			y producciones a NULL (necesario para llamar a realloc())
	for (i=0;i<NUM_TRIMESTRES;i++) {
		num_bloques[i] = 0;
		o->n[i] = 0;
		o->produccion[i] = NULL;
		o->velocidad[i] = NULL;
	}
	
	// Almacenamos cada registro contenido en cada linea del archivo hasta que
	// encontramos la palabra predicciones
	while (1) { 
		if( fgets(linea,TAM_LINEA_FICHERO,fp) == NULL ) {
			// Se produjo un error o se alcanzo el final de fichero
			if(feof(fp)) 
				break;
			else
				return -1;
		}

		// fgets pone el caracter de nueva linea. Lo eliminamos
		linea[strlen(linea)-1] = 0;

		// Comprobamos que no es la palabra predicciones
		if (strcmp(linea,"predicciones") == 0) {
			break;
		}
		
		// Separamos los datos de la linea en sus variables
		sscanf(linea,"%d-%d-%d %d:%d %f %f\0",&ano,&mes,&dia,&hora,&minuto,&prod,&vel);


		// Calculamos el trimestre a partir del mes
		trimestre = (mes - 1 - (mes - 1)%3)/3;

		// Reservamos memoria dinamicamente en funcion del numero de registros.
		// Se hace en bloques para no llamar excesivamente a realloc().
		// Se reserva memoria para BLOQUE registros. Si no es suficiente para 2*BLOQUE registros. Si no para 3*BLOQUE...
		if (o->n[trimestre]%BLOQUE == 0) {
			num_bloques[trimestre]++;
			if((o->produccion[trimestre] = 
				(float *)realloc(o->produccion[trimestre],sizeof(float)*BLOQUE*num_bloques[trimestre])) == NULL) {
				return -1;
			}
		
			if((o->velocidad[trimestre] = 
				(float *)realloc(o->velocidad[trimestre],sizeof(float)*BLOQUE*num_bloques[trimestre])) == NULL) {
				return -1;
			}			
		}

		// Asignamos los valores leidos del archivo en la estructura de datos
		o->n[trimestre]++;
		o->produccion[trimestre][o->n[trimestre] - 1] = prod;
		o->velocidad[trimestre][o->n[trimestre] - 1]  = vel;
	}

	// Se cierra el fichero
	if ( fclose(fp) == EOF ) {
		return -1;
	}

	// Fin con exito
	return 0;
}

/*
 Funcion:	cargarDatosPredicciones
 Tarea:		Lee las predicciones de un fichero de texto y los almacena en memoria
 Parametros:
			fichero: entrada	puntero al inicio de la cadena del nombre del archivo
			datos:	 salida		puntero a la estructura donde se almacenan los datos de 
								los registros empleado para hacer las predicciones
*/

int cargarDatosPredicciones(char fichero[], struct sPredicciones *p) {
	FILE *fp;						// Puntero para el manejo del archivo
	char linea[TAM_LINEA_FICHERO];	// cadena de donde se almacena las lineas que se leen del archivo

	unsigned int ano;				// Ano del registro leido
	unsigned int mes;				// Mes del registro leido
	unsigned int dia;				// Dia del registro leido
	unsigned int hora;				// Hora del registro leido
	unsigned int minuto;			// Minutos del registro leido
	float vel;						// Velocidad del viento del registro leido

	unsigned int num_bloques;		// Numero de bloques reservados cuando la reserva de memoria dinamica
									// durante la lectura sucesiva de los registros en el fichero de texto

	struct tm *aux_t;				// Estructura auxiliar para almacenar los datos de tiempo de un registro
	time_t	  tiempo;				// Variable auxiliar para manejar la conversion a time_t

	// Abrimos el fichero
	if ((fp = fopen(fichero,"r")) == NULL) {
		return -1;
	}

	// Buscamos la linea que tiene la palabra "predicciones"
	// La siguiente linea a la palabra predicciones contiene la primera linea de datos
	do{ 
		if( fgets(linea,TAM_LINEA_FICHERO,fp) == NULL ) {
			// Se produjo un error o se llego a EOF sin encontrar la palabra
			return -1;
		}

		// fgets tambien pone el caracter de nueva linea. Lo eliminamos
		linea[strlen(linea)-1] = 0;
	} while(strcmp(linea,"predicciones") != 0);

	// Ponemos los datos a un estado inicial
	// Numero de bloques a 0
	// Numero de elementos a 0 
	// El puntero al inicio del vector de registros a NULL (necesario para llamar a realloc())
	num_bloques = 0;
	p->n = 0;
	p->registros = NULL;
	
	// Leemos cada linea del archivo hasta el final y almacenamos en memoria los registros
	while (1) { 
		if( fgets(linea,TAM_LINEA_FICHERO,fp) == NULL ) {
			// Se produjo un error o se alcanzo el final de fichero
			if(feof(fp)) 
				break;
			else
				return -1;
		}

		// fgets pone el caracter de nueva linea al final de la cadena. Lo eliminamos
		linea[strlen(linea)-1] = 0;

		// Separamos los datos de la linea en sus variables
		sscanf(linea,"%d-%d-%d %d:%d %f\0",&ano,&mes,&dia,&hora,&minuto,&vel);

		// Reservamos memoria dinamicamente en funcion del numero de registros.
		// Se hace en bloques para no llamar excesivamente a realloc().
		// Se reserva memoria para BLOQUE registros. Si no es suficiente para 2*BLOQUE registros. Si no para 3*BLOQUE...
		if (p->n%BLOQUE == 0) {
			num_bloques++;
			if((p->registros = (struct sRegistro *)realloc(p->registros,sizeof(struct sRegistro)*BLOQUE*num_bloques)) == NULL) {
				return -1;
			}
		}

		// Se convierte los datos ano,mes,dia,hora,minuto a t_time
		tiempo = time(NULL);
		if (tiempo == -1) {
			return -1;
		}

		aux_t = localtime(&tiempo);
	
		aux_t->tm_year = ano - 1900;
		aux_t->tm_mon  = mes - 1;
		aux_t->tm_mday = dia;
		aux_t->tm_hour = hora;
		aux_t->tm_min  = minuto;
		aux_t->tm_sec  = 0;
		aux_t->tm_isdst = -1;
		tiempo = mktime(aux_t);

		if (tiempo == -1) {
			return -1;
		}

		// Asignamos los valores leidos del archivo en la estructura de datos
		p->n++;		
		p->registros[p->n - 1].tiempo = tiempo;
		p->registros[p->n - 1].velocidad = vel;
	}

	// Se cierra el fichero
	if ( fclose(fp) == EOF ) {
		return -1;
	}

	// Fin con exito
	return 0;
}

/*
 Funcion:	imprimirPrediciones
 Tarea:		Imprime el valor de las prediciones con el siguiente formato
				aaaa-mm-dd hh:mm potencia(sin decimales)
 Parametros: 
			p:	entrada		estructura donde se almacenan los punteros a las observaciones
*/

void imprimirPredicciones(const struct sPredicciones *p) {
	unsigned int i;
	char buffer[25];
	struct tm * timeinfo;	

	for (i=0;i<p->n;i++) {
		timeinfo = localtime ( &p->registros[i].tiempo );
		strftime(buffer,80,"%Y-%m-%d %H:%M ",timeinfo);
		printf(buffer);
		printf("%.0f\n",p->registros[i].produccion);
	}
}

/*
 Funcion:	borrarDatosObservaciones
 Tarea:		Libera la memoria empleada para almacenar las observaciones
 Parametros: 
			datos:	entrada		puntero a la estructura donde se almacenan los datos de 
								de las observaciones
*/

void borrarDatosObservaciones(const struct sObservaciones *o) {
	unsigned int i;

	for (i=0;i<NUM_TRIMESTRES;i++) {
		free(o->velocidad[i]);
		free(o->produccion[i]);
	}
}

/*
 Funcion:	borrarDatosObservaciones
 Tarea:		Libera la memoria empleada para almacenar las predicciones
 Parametros:
			datos:	entrada		puntero a la estructura donde se almacenan los datos de 
								los registros empleado para hacer las predicciones
*/

void borrarDatosPredicciones(const struct sPredicciones *p) {
	free(p->registros);
}