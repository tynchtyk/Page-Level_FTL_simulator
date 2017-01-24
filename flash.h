#include "config.h"

#define FTL_SUCCESS			0
#define FTL_READ_ERROR		1
#define	FTL_WRITE_ERORR		2
#define FTL_ERASE_ERROR		3
#define FTL_RANGE_ERROR		4


void init_flash(void);
int read_page(blk_t blkno, in_page_t pageno, pbuf_t buf, pbuf_t spare);
int write_page(blk_t blkno, in_page_t pageno, pbuf_t buf, pbuf_t spare);
int erase_block(blk_t blkno);
void print_exe_time(void);