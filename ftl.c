#include "ftl.h"
#include <memory.h>
#include <stdio.h>
typedef in_page_t	vcount;

vcount	valid_count[NUM_BLOCKS];

blk_t	empty_blk, write_blk;

page_t  map_table[58982];

in_page_t write_offset; // write offset in a write block

int ftl_open(void) {

	memset(valid_count, 0x00, sizeof(valid_count));
	memset(map_table, 0xFF, sizeof(map_table));

	init_flash();

	write_blk = 0;
	empty_blk = 1;

	write_offset = 0;

	if( erase_block(write_blk) )
		return FTL_ERROR;

	return FTL_SUCCESS;
}

int ftl_read_sector(sector_t sectorno, pbuf_t buf) {

	blk_t blk;
	in_page_t in_page;
	buf_t	spare_buf[SPARE_SIZE];

	blk = map_table[sectorno] / PPB;
	in_page = map_table[sectorno] % PPB;    // to be filled
	//printf("%d\n", blk);
	//printf("%c\n", in_page);
	if( read_page(blk, in_page, buf, spare_buf) != FTL_SUCCESS ) {
		//printf("problem in read_page\n");
		return FTL_ERROR;
	}
//		printf("%d  ", sectorno);
// 		printf("%s \n",*((page_t*)spare_buf);

	if( sectorno != *((page_t*)spare_buf) ) {
//		printf("sectorno != *((page_t*)spare_buf\n");
//		printf("%d\n", sectorno);
//		printf("%s\n", spare_buf);		
		return FTL_ERROR;
	}

	return FTL_SUCCESS;
}

// this function has to be called by ftl_garbagte_collection()
// greedy policy is implemented
static blk_t find_target_blk(void) {
	blk_t blk, min_blk;
	vcount min_vcount = PPB+1;

	for( blk = 0; blk < NUM_BLOCKS; blk++ ) {
		if(blk != write_blk && blk != empty_blk && min_vcount > valid_count[blk]) {
			min_blk = blk;
			min_vcount = valid_count[blk];
		}
		// to be written (find min valid blk except write_blk & empty_blk)
	}

	return min_blk;
}

static int ftl_garbage_collection(void)  {

	blk_t target_blk, sector_blk;
	in_page_t in_page, sector_in_page, copied_pages;
	page_t	sectorno;
	buf_t data_buf[DATA_SIZE], spare_buf[SPARE_SIZE];

	target_blk = find_target_blk();
	//printf("%d\n", target_blk);
	copied_pages = 0;

	if( erase_block(empty_blk) != FTL_SUCCESS ) {
		printf("erase_blk debil\n");
		return FTL_ERROR;
	}

	if( valid_count[target_blk] > 0 ) {

		for( in_page = 0; in_page < PPB; in_page++ ) {

			if( read_page(target_blk, in_page, data_buf, spare_buf) != FTL_SUCCESS ) {
				return FTL_ERROR;			
			}		

			sectorno = *( (page_t*)spare_buf);

			sector_blk = map_table[sectorno] / PPB;
			sector_in_page = map_table[sectorno] % PPB; // to be filled

			if( target_blk == sector_blk && sector_in_page == in_page ) {
				// the page is valid
				
				if( write_page( empty_blk, copied_pages, data_buf, spare_buf) != FTL_SUCCESS )
					return FTL_ERROR;			

				map_table[sectorno] = empty_blk * PPB + copied_pages;

				copied_pages++;
			}
		}

		if( valid_count[target_blk] != copied_pages ){
			printf("valid_count != copypages\n");
			return FTL_ERROR;
		}

		valid_count[target_blk] = 0;
		valid_count[empty_blk] = copied_pages;
	}

	write_blk = empty_blk;
	write_offset = copied_pages;
	/*if(copied_pages >= 64) {
			printf("vot suka\n");
	}*/

	empty_blk = target_blk;// to be filled (what is next status for target block?)
//	target_blk++;
	return FTL_SUCCESS;
}

int ftl_write_sector(sector_t sectorno, pbuf_t buf) {
	//printf("write_sector called\n");
	blk_t old_blk;
	buf_t spare_buf[SPARE_SIZE];

	if( write_offset >= PPB ) {
	//	printf("garbage_collection is called\n");
		ftl_garbage_collection();
	}
	if(write_offset >= PPB) {
		printf("che za nafug\n");
	}
	if( map_table[sectorno] != (page_t)~0 ) {
		//printf("zero from beginning\n");
		old_blk = map_table[sectorno] / PPB;

		if( valid_count[old_blk] == 0 ) {
			printf("error in valid_count \n");			
			return FTL_ERROR;
		}
		valid_count[old_blk]--; // To be written (manage valid count)
	}

	*( (page_t*)spare_buf) = sectorno;

	//printf("%s\n", spare_buf);
	if( write_page(write_blk, write_offset, buf, spare_buf) != FTL_SUCCESS ) {
	//	printf("%d\n", write_blk);			
	//	printf("error in write_page\n");			
	//	printf("%d %d\n",write_blk, write_offset);
		return FTL_ERROR;
	}// To be written (call write_page func.) 

	write_offset++;

	map_table[sectorno] = write_blk * PPB + write_offset-1;// To be wriiten (manage map_table)
	//printf("%d ", write_offset);
	//printf("%d\n", valid_count[write_blk]);

	valid_count[write_blk]++;// To be written (manage valid count)

	if( valid_count[write_blk] > PPB ) {
	//	printf("valid_count > PPB\n");
		return FTL_ERROR;	
	}

	return FTL_SUCCESS;
}

int ftl_close(void) {

	print_exe_time();

	return FTL_SUCCESS;
}


int ftl_test_internal(void) {
	vcount tmp_vcount[NUM_BLOCKS];
	page_t pageno;
	blk_t blk;
	in_page_t in_page;
	buf_t	data_buf[DATA_SIZE], spare_buf[SPARE_SIZE];

	memset(tmp_vcount, 0x00, sizeof(tmp_vcount));


	for( pageno = 0; pageno < LOGICAL_PAGES; pageno++ ) {
		if( pageno == 8339 )
			pageno = pageno;

		blk = map_table[pageno] / PPB;
		in_page = map_table[pageno] % PPB;

		tmp_vcount[blk]++;

		if( read_page(blk, in_page, data_buf, spare_buf) != FTL_SUCCESS ) {
			return FTL_ERROR;
		}

		if( pageno != *((page_t*)spare_buf) )
			return FTL_ERROR;
	}

	for( blk = 0; blk < NUM_BLOCKS; blk++ ) {
		if( tmp_vcount[blk] != valid_count[blk] )
			return FTL_ERROR;
	}

	return FTL_SUCCESS;
}
