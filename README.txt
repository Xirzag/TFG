Existen tres carpetas:

# La carpeta Demostrador contiene el proyecto en Visual Studio 2017 del demostrador de la realidad virtual. Contiene tanto las clases de visión estereoscópica desarrolladas, como las librerías necesarias para compilar el proyecto. El programa compilado, tanto del ejemplo de la memoria como el propio demostrador se encuentran en la carpeta Release junto con las librerías dinámicas necesarias para su ejecución.
En la carpeta Debug se encuentran sus versiones sin optimizar. 
En la carpeta TFG se encuentra el código del demostrador mientras que en la carpeta Example el código de ejemplo del anexo de la memoria.

Para lanzar el demostrador se debe usar los parámetros desde la carpeta TFG
$..\Release\Demo.exe ipDelDispositivo puertoDelDispositivo pantallaDondeMostrarLaRV
$ Demostrador\TFG> ..\Release\Demo.exe 192.168.1.100 2700 1

# La carpeta SensorTransmitter contiene el proyecto en Android Studio de la aplicación desarrollada para dispositivos Android. Simplemente importando el proyecto a Android Studio debería ser posible trabajar con él. La aplicación generada 'SensorTransmitter.apk' está en la raiz del proyecto.
Como es habitual el código de la aplicación estará en la ruta 'app/src/main/java/com/sensortransmitter'

# La carpeta Imágenes contiene imágenes realizadas para la memoria y capturas de pantalla de la realidad virtual.

