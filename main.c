#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "IO.h"
#include "operaciones.h"

int main(int argc, char **argv){
	struct sObservaciones o;				// Estructura para almacenar los datos de las observaciones
	struct sPredicciones p;					// Estructura para almacenar los datos de las predicciones
	char *fichero;							// Puntero al inicio de la cadena que contiene la ruta y 
											// el nombre del fichero. Su valor se pasa primer argumento de
											// la aplicacion

	float pendiente;						// Pendiente de la recta de regresion
	float ordenada;							// Ordenada de la recta de regresion
	float errorTrimestres[NUM_TRIMESTRES];	// Vector donde se almacena el error por trimestre

	unsigned int i;

	// Comprobamos el numero de argumentos
	if (argc != 2)  {
			fprintf(stderr,"Uso: <nombre del fichero>\n");
			exit(1);
	}

	fichero = argv[1];

	// Cargamos los datos de las observaciones
	if (cargarDatosObservaciones(fichero, &o) == -1) {
		fprintf(stderr,"Error al cargar las observaciones.\n");
		exit(1);
	}

	// Con los datos de las observaciones calculamos la recta de prediccion
	regresionLineal(&pendiente, &ordenada, &o);
	if (errno == EDOM) {
		fprintf(stderr,"Error al hacer la regresion lineal con los datos de las observaciones.\n");
		exit(1);
	}

	// Calculamos el error que se comete al predecir por una recta para cada cuatrimestre
	for(i=0;i<NUM_TRIMESTRES;i++) {
		errorTrimestres[i] = errorRegresionTrimestre(pendiente, ordenada, &o, i);
		if (errno == EDOM) {
			fprintf(stderr,"Error al calcular el error de regresion del trimestre %d.\n",i);
		}
	}

	// Cargamos en los datos con los que hay que hacer predicciones
	if (cargarDatosPredicciones(fichero, &p) == -1) {
		fprintf(stderr,"Error al cargar los datos para hacer las predicciones.\n");
		exit(1);
	}

	calcularPredicciones(&p, pendiente, ordenada);

	// Imprimimos los resultados
	// Pendiente y ordenada con 2 decimales de precision
	printf("%.2f %.2f\n",pendiente,ordenada);

	// Error cometido por trimestre (1,2,3,4) con dos decimales de precision
	for (i=0;i<NUM_TRIMESTRES;i++) {
		printf("Error trimestre %d: %.2f\n",i+1,errorTrimestres[i]);
	}

	// Predicciones
	// Se ordenan los datos de antes a despues de acuerdo a la fecha y hora
	ordenarDatosPredicciones(&p);
	imprimirPredicciones(&p);


	// Liberamos la memoria reservada para los datos
	borrarDatosObservaciones(&o);
	borrarDatosPredicciones(&p);

	system("pause");
	return 0;
}