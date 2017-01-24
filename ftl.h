#include "config.h"
#include "flash.h"

#define FTL_SUCCESS			0
#define FTL_ERROR			1

#define SECTOR_RANGE		0.9

#define LOGICAL_PAGES		((page_t)(TOTAL_PAGES*SECTOR_RANGE))

#define TOTAL_SECTORS		LOGICAL_PAGES


int ftl_open(void);
int ftl_read_sector(sector_t sectorno, pbuf_t buf);
int ftl_write_sector(sector_t sectorno, pbuf_t buf);
int ftl_close(void);
int ftl_test_internal(void);