# Visualizador de particiones de discos MBR/GPT
Elaborado por:
- Erwin Meza Vega <emezav@unicauca.edu.co>
- Jonathan David Guejia <jonathanguejia@unicauca.edu.co>
- Jhoan David Chacon <jhoanchacon@unicauca.edu.co>

# Proyecto: Lectura de la Tabla de Particiones en Dispositivos de Disco

Este proyecto consiste en desarrollar un programa en C que, a partir de la ruta de un dispositivo tipo disco, imprime su tabla de particiones. El programa debe ser capaz de detectar el esquema de particionado y mostrar detalles relevantes para cada partición. 

## Requisitos

El programa tomará como entrada la ruta de un dispositivo tipo disco y deberá mostrar la siguiente información:

### Esquema de particionado
- **MBR** (Master Boot Record)
- **GPT** (GUID Partition Table)

### Información de cada partición
Para cada partición en la tabla, el programa deberá imprimir los siguientes detalles:
- **Tipo de partición**: El tipo de partición especificado en la tabla de particiones.
- **Sector inicial**: El sector donde comienza la partición en el dispositivo de almacenamiento.
- **Sector final**: El sector donde termina la partición.
- **Tamaño en bytes**: El tamaño total de la partición en bytes, calculado a partir de los sectores.
