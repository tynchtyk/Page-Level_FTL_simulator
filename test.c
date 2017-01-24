#include "ftl.h"
#include <memory.h>
#include <stdio.h>


int main(void) {
	FILE* ifp;
	char type_c;
	sector_t sectorno; // int
	buf_t	buffer[DATA_SIZE];
	buf_t	cur[DATA_SIZE];

	if( !(ifp = fopen("w3_mixed.txt", "r")) ) {
		printf("trace file open error.\n");
		return 1;
	}

	ftl_open();	
	//printf("%d\n", TOTAL_SECTORS);

	for( sectorno = 0; sectorno < TOTAL_SECTORS; sectorno++ ) {
		*((sector_t*)buffer) = sectorno;
		//printf("%s\n", buffer);
		//printf("%s\n", buffer);
		if( ftl_write_sector(sectorno, buffer) != FTL_SUCCESS ) {

			printf("error in the beginnig");
	
			return 3;
		}
	}
/*	for( sectorno = 0; sectorno < TOTAL_SECTORS; sectorno++ ) {
		*((sector_t*)cur) = sectorno;	
		if(ftl_read_sector(sectorno, buffer) != FTL_SUCCESS ) {

			printf("error in the beginnig");
	
			return 3;
		}
		printf("%s ", cur);
		printf("%s\n", buffer);
	}*/
	
	int index = 1;
	while( !feof(ifp) ) {

		fscanf(ifp, "%c %u\n", &type_c, &sectorno);

		sectorno = sectorno % TOTAL_SECTORS;

		if( type_c == 'R' ) {
//			printf("first read is called\n");
			if( ftl_read_sector(sectorno, buffer) != FTL_SUCCESS ) {

	//			printf("%s\n", buffer);
				printf("%d index\n", index);
				return 4;
			}
		}
		else if( type_c == 'W' ) {
			if( ftl_write_sector(sectorno, buffer) != FTL_SUCCESS ) {
				printf("blyad5.\n");
				return 5;
			}
		}
		else {
			printf("something wrong.\n");
			return 2;
		}
		index ++;
	}

	if( ftl_test_internal() != FTL_SUCCESS ) {
		printf("blyad6.\n");
		return 6;
	}
	else {
		printf("FTL Test successfully finished!\n\n");
	}

	ftl_close();

	fclose(ifp);

	return 0;
}