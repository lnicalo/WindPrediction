#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <time.h>

#include "operaciones.h"
#include "IO.h"

/*
 Funcion:	regresionLineal
 Tarea:		Calcula por minimos cuadrados la pendiente y la oseconds elapsed since 00:00 hours, Jan 1, 1970 UTCrdenada de una recta de regresion lineal.
						
			pendiente = (N*sxy - sx*sy)/(N*sxx - sx*sx)
			ordenada  = sy - *pendiente*sx

			donde
				sx		Media de los datos x - 1/N*sum(x)
				sy		Media de los datos y - 1/N*sum(y)
				sxy		Correlacion de los datos x e y 1/N^2*sum(x*y)
				sxx		Valor cuadratico de los datos de x 1/N^2*sum(x*x)

				N		Es el numero de datos

			En caso de error al calcular se modifica la variable global errno = EDOM.

 Parametros:
			 obs: entrada	puntero a la estructura que contiene las observaciones con los datos
					x <= velocidad
					y <= potencia
			 peniente: salida	pendiente de la recta de regresion
			 ordenada: salida	ordenada de la recta de regresion
*/

void regresionLineal(float *pendiente, float *ordenada, const struct sObservaciones *obs) {
	float sx;		// Media de los datos x - 1/N*sum(x)
	float sy;		// Media de los datos y - 1/N*sum(y)
	float sxy;		// Correlacion 1/N^2*sum(x*y)
	float sxx;		// Valor cuadratico 1/N^2*sum(x*x)
	
	float cociente; // Variable auxiliar
	
	unsigned int N;
	unsigned i;

	// Numero de datos empleados en la regresion
	N = 0;
	for (i=0;i<NUM_TRIMESTRES;i++) {
		N += obs->n[i];
	}

	// Calculamos la pendiente y la ordenada
	sx = media((const float **)obs->velocidad,obs->n);
	sy = media((const float **)obs->produccion,obs->n);
	sxx = vcm((const float **)obs->velocidad,(const unsigned int *)obs->n);
	sxy = rxy((const float **)obs->velocidad,(const float **)obs->produccion,(const unsigned int  *)obs->n);
		
	// Se comprueba que no dividimos por cero
	cociente = (N*sxx - sx*sx);
	if (cociente == 0) {
		errno = EDOM;
		return;
	}

	*pendiente = (N*sxy - sx*sy)/cociente;
	*ordenada = sy - *pendiente*sx;

	return;
}

/*
 Funcion:	errorRegresionTrimestre
 Tarea:		Calcula el error medio de un trimestre que se comete en el ajuste al emplear
			la recta de regresion en comparacion con los datos de produccion proporcionados

			El error medio se calcula mediante la siguiente expresion:
				error medio = sum(abs(observacion - prediccion))/n

			En caso de error al calcular debido a que n = 0 se modifica la variable global 
			errno = EDOM y se devuelve 0 pero no tiene ningun significado.
			
			La funcion devuelve un float con el error medio cometido en el trimestre seleccionado
 Parametros:
			 pendiente: entrada		pendiente de la recta de regresion
			 ordenada:  entrada		ordenada de la recta de regresion
			 obs:		entrada		puntero a la estructura que contiene las observaciones
			 trimestre:	entrada		trimestre en el que queremos calcular el error. Puede tomar valor
									entre 0 y NUM_TRIMESTRES-1 
*/

float errorRegresionTrimestre(float pendiente, float ordenada, const struct sObservaciones *obs, unsigned int trimestre) {
	unsigned int i;
	float error;

	// Si el numero de elementos es cero salimos con error
	if (obs->n[trimestre] == 0) {
			errno = EDOM;
			return 0;
	}

	error = 0;
	for(i=0;i<obs->n[trimestre];i++) {
		error += (float)fabs(obs->produccion[trimestre][i] - pendiente*obs->velocidad[trimestre][i] - ordenada);
	}
	
	return error/obs->n[trimestre];
}

/*
 Funcion:	calcularPredicciones
 Tarea:		Calcula la estimacion de la potencia a partir de la velocidad
			
 Parametros:
			 pendiente: entrada					pendiente de la recta de regresion que se quiere emplear para la prediccion
			 ordenada:  entrada					ordenada de la recta de regresion que se quiere emplear para la prediccion
			 datos:		entrada - salida		puntero a la estructura que contiene los datos para las predicciones. 
												En esta estructura se almacena el valor de la prediccion calculada

*/

void calcularPredicciones(struct sPredicciones *datos, float pendiente, float ordenada) {
	unsigned int i;

	for(i=0;i<datos->n;i++) {
		datos->registros[i].produccion = pendiente*datos->registros[i].velocidad + ordenada;
	}
}

/*
 Funcion:	ordenarDatosPredicciones
 Tarea:		Ordena los registros almacenados en la estructura sPredicciones
			por orden de fecha y hora. En primer lugar los que se grabaron en
			en primer lugar en el tiempo.

 Parametros:
			datos: entrada - salida		puntero a la estructura que contiene los datos a ordenar
*/

void ordenarDatosPredicciones(struct sPredicciones *datos) {
	qsort (datos->registros, datos->n, sizeof(struct sRegistro), comparar);
}

/*
 Funcion:	media
 Tarea:		Calcula la media de todos los elementos de 'x'
			La funcion devuelve la media. En caso de error se modifica la variable global 
			errno = EDOM y se devuelve 0 pero no tiene ningun significado. 
 Parametros:
			x: entrada		vector de tamano NUM_TRIMESTRES que debe contener los punteros que apuntan al inicio 
							del vector de datos de velocidad o produccion de cada trimestre.
			n: entrada		vector de tamano NUM_TRIMESTRES que debe contener el numero de elementos de cada uno
							de los vectores a los que apuntan los punteros almacenados en x
*/

float media(const float **x, const unsigned int *n) {
	unsigned int i,j;
	float suma = 0;
	unsigned int numel = 0;

	for (i=0;i<NUM_TRIMESTRES;i++) {
		for (j=0;j<n[i];j++) {
			suma += x[i][j];
			numel++;
		}
	}

	// Nos aseguramos de no dividir por cero
	if (numel == 0) {
		errno = EDOM;
		return 0;
	}

	return suma/numel;
}

/*
 Funcion:	vcm
 Tarea:		Calcula el valor cuadratico medio de todos los elementos de 'x'
			La funcion devuelve el valor cuadratico medio. En caso de error se modifica la variable global 
			errno = EDOM y se devuelve 0 pero no tiene ningun significado. 
 Parametros:
			x: entrada		vector de tamano NUM_TRIMESTRES que debe contener los punteros que apuntan al inicio 
							del vector de datos de velocidad o produccion de cada trimestre.
			n: entrada		vector de tamano NUM_TRIMESTRES que debe contener el numero de elementos de cada uno
							de los vectores a los que apuntan los punteros almacenados en x
*/

float vcm(const float **x,const unsigned  int *n) {
	unsigned int i,j;

	float suma = 0;
	unsigned int numel = 0;

	for (i=0;i<NUM_TRIMESTRES;i++) {
		for (j=0;j<n[i];j++) {
			suma += x[i][j]*x[i][j];
			numel++;
		}
	}

	// Nos aseguramos de no dividir por cero
	if (numel == 0) {
		errno = EDOM;
		return 0;
	}

	return suma/(numel*numel);
}

/*
 Funcion:	rxy
 Tarea:		Calcula el la covarianza entre los elementos de 'x' y de 'y'
			La funcion devuelve la covarianza. En caso de error se modifica la variable global 
			errno = EDOM y se devuelve 0 pero no tiene ningun significado. 
 Parametros:
			x: entrada		vector de tamano NUM_TRIMESTRES que debe contener los punteros que apuntan al inicio 
							del vector de datos de velocidad o produccion de cada trimestre.
			y: entrada		vector de tamano NUM_TRIMESTRES que debe contener los punteros que apuntan al inicio 
							del vector de datos de velocidad o produccion de cada trimestre.
			n: entrada		vector de tamano NUM_TRIMESTRES que debe contener el numero de elementos de cada uno
							de los vectores a los que apuntan los punteros almacenados en x
*/

float rxy(const float **x, const float **y, const unsigned int *n) {
	unsigned int i,j;

	float suma = 0;
	unsigned int numel = 0;
	for (i=0;i<NUM_TRIMESTRES;i++) {
		for (j=0;j<n[i];j++) {
			suma += x[i][j]*y[i][j];
			numel++;
		}
	}

	// Nos aseguramos de no dividir por cero
	if (numel == 0) {
		errno = EDOM;
		return 0;
	}

	return suma/(numel*numel);
}

/*
 Funcion:	comparar
 Tarea:		Compara dos registros de la estructura sPrediciones en funcion del instante en el que fue
			registrado.
			Si a1 fue registrado anteriormente a a2 devuelve un numero entero < 0
			Si a1 fue registrado en el mismo instante de tiempo que a2 devuelve un entero igual a 0
			Si a1 fue registrado despues que a2 devuelve un numero > 0

 Parametros:
			a1:	entrada		puntero al registro 1 que se quiere comparar
			a2:	entrada		puntero al registro 2 que se quiere comparar
*/

int comparar(const void *a1, const void  *a2) {
	struct sRegistro *r1 = (struct sRegistro *)a1;
	struct sRegistro *r2 = (struct sRegistro *)a2;

	return (int)r1->tiempo - (int)r2->tiempo;
}

