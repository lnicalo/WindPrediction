Wind Prediction 
==============

Wind prediction - linear regression

Se desea elaborar un sencillo sistema de predicción de producción de energía para parques eólicos. Para ello el candidato debe programar una sencilla aplicación que permita obtener una relación entre la producción de energía de un parque eólico y el viento. Esta relación podrá ser diferente para cada parque eólico, por lo que se necesita un sistema automático para obtener y evaluar la relación para diferentes parques eólicos.

En este caso vamos a suponer que hubiera una relación lineal entre módulo de velocidad de viento y la producción de energía. A través de los datos de producción de energía y el módulo de velocidad de viento se trata de obtener una recta que proporcione la producción de energía a partir de la velocidad de viento. El ajuste de esta recta se hará por mínimos cuadrados.

El ajuste obtenido se utilizará para dos cosas:
1.- Calcular el error medio absoluto, sum(abs(observacion – prediccion))/n, por trimestre que se
está cometiendo con ese ajuste.
2.- Realizar nuevas predicciones.

Entrada de datos
La entrada de datos consistirá en dos bloques. El primero comenzará con la palabra observaciones. En la siguiente línea tendremos la fecha en formato YYYY-MM-DD hh:mm y la observación de producción junto con el módulo de la velocidad de viento. Como máximo se incluirán las observaciones de un año, pero pueden ser menos. El segundo bloque estará delimitado por la palabra predicciones y contendrá lineas con la fecha y hora y el módulo de la velocidad de
viento. La entrada terminará con el carácter de fin de archivo. Las observaciones y las predicciones de entrada pueden no estar ordenadas. Ejemplo:

```
observaciones
2008-01-25 10:00 1200 8
2008-01-15 11:00 1150 7.5
predicciones
2009-10-25 23:00 4
2009-10-20 14:00 5
```

Salida de datos
La salida de datos consistirá en varias partes. La primera parte será la recta de ajuste, primero la pendiente y luego la ordenada en el origen, con dos decimales de precisión. La segunda parte consistirá en el error por trimestre que se está cometiendo en ese ajuste respecto a los datos de producción proporcionados, con dos decimales de precisión. La última parte consistirá en la predicción que se daría de producción para cada uno de los valores introducidos en la parte predicciones de la entrada, pero de forma ordenada, sin decimales. Ejemplo:

```
100.00 400.00
Enero 0.00
2009-10-20 14:00 900
2009-10-25 23:00 800
```

En el archivo adjunto datos_problema.txt hay unos datos de ejemplo como entrada del programa.
La solución se puede realizar tanto en C como en C++. No se puede utilizar ninguna biblioteca
que no forme parte del estándar.
Criterios de valoración:
1. Corrección
2. Eficiencia
3. Documentación y organización del código
