/**
 * @file main.c
 * @brief Listar particiones de discos duros MBR/GPT
 * @author Jhoan David Chacón <jhoanchacon@unicauca.edu.co>
 * @author Jonathan David Guejia <jonathanguejia@unicauca.edu.co>
 * @author Erwin Meza Vega <emezav@unicauca.edu.co>
 * @copyright MIT License
*/
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "mbr.h"
#include "gpt.h"

/** @brief Sector size */
#define SECTOR_SIZE 512

/**
* @brief Hex dumps a buffer
* @param buf Pointer to buffer
* @param size Buffer size
*/
void hex_dump(char * buf, size_t size);

/**
* @brief ASCII dumps a buffer
* @param buf Pointer to buffer
* @param size Buffer size
*/
void ascii_dump(char * buf, size_t size);
/**
 * @brief Read a sector from a disk
 * 
 * @param disk Disk filename
 * @param lba Sector to read (0 - amount of LBA sectors on disk)
 * @param buf Buffer to read the sector info
 * @return int 1 on success, 0 on failure
 */
int read_lba_sector(char * disk, unsigned long long lba, char buf[512]);

/**
 * @brief Prints the partition table of a MBR
 * 
 * @param boot_record MBR boot record
 */
void print_partition_table(mbr * boot_record);
/**
 * @brief Prints the header of a GPT
 * 
 * @param hdr GPT header
 */
void print_gpt_header(gpt_header * hdr);

/**
 * @brief Prints the descriptor of a GPT partition
 * 
 * @param desc GPT partition descriptor
 */
void print_partition_descriptor(gpt_partition_descriptor * desc);

/**
 * @brief Table titles design
 * 
 */
void titlesTable();

/**
 * @brief Converts a string to uppercase
 * 
 * @param str String to convert
 */
void to_upper(char *str);

int num_sectors=0; /*Cantidad de sectores de la tabla (cantidad de entradas x tamaño de cada entrada)/tamaño sector*/

int main(int argc, char *argv[]) {
	int i;
	char * disk;
	//1. Validar los argumentos de la linea de comandos
	if(argc<2){
		fprintf(stderr,"Usage: %s disk1 [disk2 ...]\n",argv[0]);
		exit(EXIT_FAILURE);
	}
	//2.Iterar sobre los discos especificados	
	for(i =1; i<argc; i++){
		//3. Leer el primer sector del disco especificado
		mbr boot_record;
		disk = argv[i];
		//3.1. Si la lectura falla, mostrar un mensaje de error y terminar
		if(read_lba_sector(disk,0,(char*)&boot_record)==0){
			fprintf(stderr,"Unable to open device\n");
			exit(EXIT_FAILURE);
		}		
		//PRE: Se pudo leer el primer sector del disco
		//4. Imprimir la tabla de particiones
		print_partition_table(&boot_record);
		//5. Si el esquema de particionado es MBR, terminar (ya se imprimió) 
		if(is_mbr(&boot_record)){
			fprintf(stderr,"This is a MBR Partition, there is no more left to do\n");
			exit(EXIT_SUCCESS);
		}
		//PRE: El esquema de particionado es GPT
		//6. Imprimir la tabla GPT
		//7. Leer el segundo sector del disco (PTHDR)
		gpt_header hdr;
		if(read_lba_sector(disk,1,(char*)&hdr)==0){
			fprintf(stderr,"Unable to read GPT header\n");
			exit(EXIT_FAILURE);
		}
		//7.1 Validar si el sector leído es un GPT Header
		if(!is_valid_gpt_header(&hdr)){
			fprintf(stderr,"Invalid GPT Header\n");
			exit(EXIT_FAILURE);
		}
		//Cantidad de sectores de la tabla de particiones
		num_sectors = ceil((hdr.num_partition_entries*hdr.size_partition_entry)/512.0);		
		//7.2 Imprimir el header
		print_gpt_header(&hdr);
		//8. Imprimir la tabla de particiones
		gpt_partition_descriptor descriptors[4]; //4 descriptores por sector(suponiendo que el tamaño del descriptor es 128 bytes y el bloque de disco es de 512 bytes)
		//Table titles
		titlesTable();
		//8.1 Leer los descriptores de las particiones
		for(int i = 0; i < num_sectors; i++){			
			//8.1.1 Leer los siguientes sectores de la tabla de particiones
			if(read_lba_sector(disk,hdr.partition_entry_lba+i,(char*)&descriptors)==0){
				fprintf(stderr,"Unable to read this sector\n");
				exit(EXIT_FAILURE);
			}			
			//8.1.2 Para cada descriptor leído, imprimir su información	
			for(int j = 0; j < 4; j++){
				//Si el descriptor es nulo, no se imprime
				if(is_null_descriptor(&descriptors[j])) continue;
				print_partition_descriptor(&descriptors[j]);
			}
		}
		printf("-------------	-------------   ------------  --------------------------------------    --------------------------------------------\n");				
	}
	return 0;
}

int read_lba_sector(char * disk, unsigned long long lba, char buf[512]) {
	FILE *fd;
	//Abrir el archivo en modo lectura
	fd = fopen(disk, "r");
	if (fd == NULL) {	
		printf("No se pudo abrir el archivo");
		return 0;
	}
	//Avanzar el apuntador de lectura en el dispositivo
	if(fseek(fd, lba * SECTOR_SIZE, SEEK_SET)!=0){
		return 0;
	}
	//Leer el sector del disco
	if(fread(buf, 1, SECTOR_SIZE, fd)!=SECTOR_SIZE){
		return 0;
	}	
	//Cerrar el archivo
	fclose(fd);
	return 1;
}

void ascii_dump(char * buf, size_t size) {
	for (size_t i = 0; i < size; i++) {
		if (buf[i] >= 0x20 && buf[i] < 0x7F) {
			printf("%c", buf[i]);
		}else {
			printf(".");
		}
	}
}

void hex_dump(char * buf, size_t size) {
	int cols;
	cols = 0;
	for (size_t i=0; i < size; i++) {
		printf("%02x ", buf[i] & 0xff);
		if (++cols % 16 == 0) {
			ascii_dump(&buf[cols - 16], 16);
			printf("\n");
		}
	}
}
void print_partition_table(mbr * boot_record) {
	char type_name[TYPE_NAME_LEN];
	if(is_mbr(boot_record)==1){
		printf("\nDisk initialized as MBR.\n");
	}else{
		printf("\nDisk initialized as GPT.\n");
	}
	printf("MBR Partition Table\n");
	printf("Start LBA\tEnd LBA\t\tType\n");
	printf("-------------	-------------   ----------------------------------\n");
	for (int i = 0; i < 4; i++) {
		//Si la partición está sin usar, no se imprime
		if (boot_record->partition_table[i].partition_type == MBR_TYPE_UNUSED) {
			continue;
		}
		//Se obtiene el nombre del tipo de partición
		mbr_partition_type(boot_record->partition_table[i].partition_type, type_name);
		//Se imprime la información de la partición
		printf("	   %d\t\t   %d\t%s\n", boot_record->partition_table[i].starting_sector_lba, boot_record->partition_table[i].sectors_in_partition, type_name);
	}
	printf("-------------	-------------   ----------------------------------\n");
}

void print_gpt_header(gpt_header * hdr){
	printf("GPT Header\n");
	printf("Revision: 0x%x\n",hdr->revision);
	printf("First usable LBA: %d\n",hdr->first_usable_lba);
	printf("Last usable LBA: %d\n",hdr->last_usable_lba);
	printf("Disk GUID: %s\n",guid_to_str(&hdr->disk_guid));
	printf("Partition Entry LBA: %d\n",hdr->partition_entry_lba);
	printf("Number of Partition Entries: %d\n",hdr->num_partition_entries);
	printf("Size of Partition Entry: %d\n",hdr->size_partition_entry);
	printf("Total of partition table entries sectors: %d\n",num_sectors);
	printf("Size of a partition Descriptor: %d\n", hdr->size_partition_entry);	
}
void print_partition_descriptor(gpt_partition_descriptor * desc){	
	//Se convierte el GUID a una cadena
	char * guid_str = guid_to_str((guid*)&desc->partition_type_guid);	
	//Tamaño en bytes de la partición
	unsigned long long size = ((desc->ending_lba - desc->starting_lba)+1)*512; 		
	//Se obtiene la información del tipo de partición a partir del GUID
	to_upper(guid_str);
	const gpt_partition_type * type = get_gpt_partition_type(guid_str);		
	printf("    %d\t",desc->starting_lba);
	printf("    %d\t",desc->ending_lba);
	printf("  %d\t",size);
	printf(" %s\t",type->description);
	printf("                %s",gpt_decode_partition_name(desc->partition_name));
	printf("\n");
}

void titlesTable(){
	printf("Start LBA\tEnd LBA\t\tSize\t\tType\t\t\t\t\tPartition name\n");
	printf("-------------	-------------   ------------  --------------------------------------    --------------------------------------------\n");
}
void to_upper(char *str) {
    while (*str) {  // Mientras no sea el carácter nulo de terminación
        *str = toupper((unsigned char)*str);  // Convierte el carácter actual a mayúscula
        str++;  // Avanza al siguiente carácter
    }
}