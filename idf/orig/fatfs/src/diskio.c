/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/* ESP-IDF port Copyright 2016 Espressif Systems (Shanghai) PTE LTD      */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "diskio.h"		/* FatFs lower layer API */
#include "ffconf.h"
#include "ff.h"

static ff_diskio_impl_t * s_impls[_VOLUMES] = { NULL }; //驱动器操作函数结构数组，记录对应驱动器的驱动函数入口，目前支持两个驱动器。

#if _MULTI_PARTITION		/* Multiple partition configuration */
PARTITION VolToPart[] = {
    {0, 0},    /* Logical drive 0 ==> Physical drive 0, auto detection */
    {1, 0}     /* Logical drive 1 ==> Physical drive 1, auto detection */
};
#endif

/* 查找未用驱动器号
    上层通过获取驱动器号，实现对该驱动器的访问。
*/
esp_err_t ff_diskio_get_drive(BYTE* out_pdrv)
{
    BYTE i;
    for(i=0; i<_VOLUMES; i++) {
        if (!s_impls[i]) {
            *out_pdrv = i;
            return ESP_OK;
        }
    }
    return ESP_ERR_NOT_FOUND;
}

void ff_diskio_register(BYTE pdrv, const ff_diskio_impl_t* discio_impl)
{
    assert(pdrv < _VOLUMES);

    if (s_impls[pdrv]) { //如果本驱动器号已挂载驱动，释放之
        ff_diskio_impl_t* im = s_impls[pdrv];
        s_impls[pdrv] = NULL;
        free(im);
    }

    if (!discio_impl) {
        return;
    }
    /* 重新分配资源，挂载新的驱动*/
    ff_diskio_impl_t * impl = (ff_diskio_impl_t *)malloc(sizeof(ff_diskio_impl_t));
    assert(impl != NULL);
    memcpy(impl, discio_impl, sizeof(ff_diskio_impl_t));
    s_impls[pdrv] = impl;
}

DSTATUS ff_disk_initialize (BYTE pdrv)
{
    return s_impls[pdrv]->init(pdrv);
}
DSTATUS ff_disk_status (BYTE pdrv)
{
    return s_impls[pdrv]->status(pdrv);
}
DRESULT ff_disk_read (BYTE pdrv, BYTE* buff, DWORD sector, UINT count)
{
    return s_impls[pdrv]->read(pdrv, buff, sector, count);
}
DRESULT ff_disk_write (BYTE pdrv, const BYTE* buff, DWORD sector, UINT count)
{
    return s_impls[pdrv]->write(pdrv, buff, sector, count);
}
DRESULT ff_disk_ioctl (BYTE pdrv, BYTE cmd, void* buff)
{
    return s_impls[pdrv]->ioctl(pdrv, cmd, buff);
}

DWORD get_fattime(void)
{
    time_t t = time(NULL);
    struct tm *tmr = gmtime(&t);
    int year = tmr->tm_year < 80 ? 0 : tmr->tm_year - 80;
    return    ((DWORD)(year) << 25)
            | ((DWORD)(tmr->tm_mon + 1) << 21)
            | ((DWORD)tmr->tm_mday << 16)
            | (WORD)(tmr->tm_hour << 11)
            | (WORD)(tmr->tm_min << 5)
            | (WORD)(tmr->tm_sec >> 1);
}