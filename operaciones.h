// operaciones.h

// Funciones
void regresionLineal(float *pendiente, float *ordenada, const struct sObservaciones *obs);
float errorRegresionTrimestre(float pendiente, float ordenada, const struct sObservaciones *obs, unsigned int trimestre);
void calcularPredicciones(struct sPredicciones *datos, float pendiente, float ordenada);
void ordenarDatosPredicciones(struct sPredicciones *datos);

// Funciones de uso interno
float media(const float **x, const unsigned int *n);
float vcm(const float **x, const unsigned int *n);
float rxy(const float **x, const float **y, const unsigned int *n);
int comparar(const void *a1, const void  *a2);