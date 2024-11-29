/**
* @file
* @brief Definiciones para discos inicializados con esquema BR
*/

#ifndef MBR_H
#define MBR_H

/** @brief Boot sector signature */
#define MBR_SIGNATURE 0xAA55

/** @brief GPT Partition type - MBR */
#define MBR_TYPE_GPT 0xEE

/** @brief Unused partition table - MBR*/
#define MBR_TYPE_UNUSED 0x00

/** @brief Longitud maxima del texto para el tipo de particion */
#define TYPE_NAME_LEN 256

/** @brief Partition descriptor - MBR */
typedef struct {
	/* La suma completa de bytes de esta estructura debe ser 16 */
	unsigned char boot_flag; //1 byte
	unsigned char initial_sector[3]; //3 bytes
	unsigned char partition_type; //1 byte
	unsigned char final_sector[3]; //3 bytes
	unsigned int starting_sector_lba; //4 bytes
	unsigned int sectors_in_partition; //4 bytes
}__attribute__((packed)) mbr_partition_descriptor;

/** @brief Master Boot Record. */
typedef struct {
	/* La suma completa de bytes de esta estructura debe ser 512 */
	//Los primeros 446 bytes son el codigo de arranque
	unsigned char loader_code[446];
	mbr_partition_descriptor partition_table[4];
	unsigned short signature;
}__attribute__((packed)) mbr;

/**
* @brief Checks if a bootsector is a MBR.
* @param boot_record Bootsector read in memory
* @return 1 If the bootsector is a MBR, 0 otherwise.
*/
int is_mbr(mbr * boot_record);

/**
* @brief Text description of a MBR partition type
* @param type Partition type reported in MBR
* @param buf String buffer to store the text description
*/
void mbr_partition_type(unsigned char type, char buf[TYPE_NAME_LEN]);


#endif
