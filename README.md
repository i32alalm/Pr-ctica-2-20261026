Práctica 2: Empleamos la UART para una comunicación serie, RS232.
===

# Objetivo de la práctica  

+ Partiendo de lo hecho en la práctica anterior...
+ Vamos a empezar a organizarnos el código aún mejor, separamos cabecera con defines, empleamos gitignore, renombramos la aplicación y modificamos Makefile.
+ Vamos a emplear una conexión UART del micro, que está cableada al COM1. De esta forma tenemos [comunicaciones serie RS232](https://en.wikipedia.org/wiki/RS-232) y podremos comunicarnos mediante paso de mensajes con nuestro sistema microcontrolador.
+ Tendremos nuestra primera toma de contacto con la velocidad de reloj cara a controlar la velocidad de las comunicaciones.
+ Cuando pulsemos el botón debemos de poder recibir una cadena de texto informando del evento.
+ ¿Podemos imprimir el número de la cuenta sin emplear aún librería estándar?

+ EXTRA: nos animamos a crearnos rutinas para la configuración de la UART1, así como para escribir caracteres? (separar el código en un uart.c/uart.h)
+ EXTRA: podríamos integrar la impresión UART como un printf estándar? Qué dificultades tenemos para emplear newlib? [syscalls reference](https://sourceware.org/newlib/libc.html), [newlib porting faq](https://sourceware.org/newlib/faq.html)


# Recordad

## OpenOCD

Para conectaros con OpenOCD deberéis ejecutar el servidor openocd **dentro de la carpeta de trabajo** mediante el comando:

(si tenéis el programador Keil ulink 2)
```
$ openocd -f interface/cmsis-dap.cfg -f mcb2300.cfg
```

(si tenéis el programador jlink, el de color negro)
```
$ openocd -f interface/jlink.cfg -f mcb2300.cfg
```

Después, en otro terminal, pero también **dentro de la carpeta de trabajo** os conectáis mediante telnet con el comando:
```
nc localhost 4444
```

Para programar la placa:
```
> reset halt
> program <nombre del ejecutable>
> reset run
```

Algunas veces la placa falla en programarse, resetearse, etc. Leed el mensaje de vuelta bien y repetid el comando que haya fallado si es necesario.

## Makefile

Editad el Makefile si añadís más ficheros de código de fuente o los renombrais.

Para borrar todos los binarios y código objeto:
```
make clean
```
## Dependencias nuevas para esta práctica

Necesitaréis instalar un terminal serie, uno clásico que funciona muy bien es minicom:
```
# emerge --ask minicom
```
```
$ sudo apt install minicom
```

Para conectarnos con minicom, habrá que detectar el puerto usb empleado por el conversor, estar en el grupo dialout de usuarios, y el comando será:
```
minicom -D /dev/ttyUSB0 -b 9600
```
\* /dev/ttyUSB= cambiará según como se mapeé el dispositivo

# Qué vamos a hacer?

Queremos que al pulsar el botón INT0 el contador BCD se incremente y el microcontrolador nos comunique un mensaje por el puerte serie.

Habrá que:

+ Configurar, en este caso porque es la más fácil, la UART 1, que está conectada al puerto COM 1.
+ Necesitaremos una comunicación de 9600 baudios y por ahora podemos evitar la partidad, bits de parada, etc.
+ vamos a crearnos unas pequeñas funciones para automatizar el envío de cadenas de texto.
+ EXTRA: podemos cambiar la velocidad de transmisión?
+ EXTRA: podemos plantear printf con su formateo para imprimir cadenas de texto?
+ EXTRA: podemos plantear una librería a alto nivel para configurar la UART?

De manera transversal vamos a:

+ emplear un control de versiones, en éste caso git.
+ vamos a empezar a facilitarnos la vida separando el código en diversos ficheros de código fuente.

# Licencia

Copyright 2026 Joaquín Cuéllar <p22cupaj@uco.es>

Este código está licenciado bajo Apache2, podéis hacer lo que queráis con él, es vuestro.