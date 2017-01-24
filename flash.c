#include "config.h"
#include "flash.h"
#include <memory.h>
#include <stdio.h>

#define READ_TIME 4
#define WRITE_TIME 12
#define ERASE_TIME 15

static buf_t flash_data[TOTAL_PAGES * DATA_SIZE];
static buf_t flash_spare[TOTAL_PAGES * SPARE_SIZE];

unsigned int g_read_count, g_write_count, g_erase_count;

in_page_t blk_offset[NUM_BLOCKS];

void init_flash(void) {
	g_read_count = g_write_count = g_erase_count = 0;
}

int read_page(blk_t blkno, in_page_t pageno, pbuf_t buf, pbuf_t spare) {
	if( blkno < NUM_BLOCKS && pageno < PPB ) {
		if( pageno >= blk_offset[blkno] )
			return FTL_READ_ERROR;

		memcpy(buf, flash_data+(blkno*BLOCK_SIZE+pageno*DATA_SIZE), DATA_SIZE);
		memcpy(spare, flash_spare+(blkno*SPARE_BLOCK_SIZE+pageno*SPARE_SIZE), SPARE_SIZE);

		g_read_count++;

		return FTL_SUCCESS;
	}
	else
		return FTL_RANGE_ERROR;
}

int write_page(blk_t blkno, in_page_t pageno, pbuf_t buf, pbuf_t spare) {
	if( blkno < NUM_BLOCKS && pageno < PPB ) {
		if( pageno < blk_offset[blkno] )
			return FTL_WRITE_ERORR;

		memcpy(flash_data+(blkno*BLOCK_SIZE+pageno*DATA_SIZE), buf, DATA_SIZE);
		memcpy(flash_spare+(blkno*SPARE_BLOCK_SIZE+pageno*SPARE_SIZE), spare, SPARE_SIZE);

		blk_offset[blkno] = pageno+1;
		g_write_count++;

		return FTL_SUCCESS;
	}
	else
		return FTL_RANGE_ERROR;
}

int erase_block(blk_t blkno) {
	if( blkno < NUM_BLOCKS ) {
		//if( blk_offset[blkno] == 0 )
		//	return FTL_ERASE_ERROR;

		memset(flash_data+(blkno*BLOCK_SIZE), 0xFF, BLOCK_SIZE);
		memset(flash_spare+(blkno*SPARE_BLOCK_SIZE), 0xFF, SPARE_BLOCK_SIZE);

		blk_offset[blkno] = 0;
		g_erase_count++;

		return FTL_SUCCESS;
	}
	else
		return FTL_RANGE_ERROR;

	return 0;
}

void print_exe_time(void) {
	double read_time, write_time, erase_time;

	read_time = (g_read_count * READ_TIME) / 10000.0;
	write_time = (g_write_count * WRITE_TIME) / 10000.0;
	erase_time = (g_erase_count * ERASE_TIME) / 10000.0;

	printf("Total elasped time: %.3lf sec\n", read_time + write_time + erase_time);
	printf("Read time: %.3lf sec, Write time: %.3lf sec, Erase time: %.3lf sec\n", read_time, write_time, erase_time);
}