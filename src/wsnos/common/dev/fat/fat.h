#ifndef __FAT_H__
#define __FAT_H__

#include "fat_defs.h"


#include "uffs/uffs.h"

#define FAT_RDWR            UO_RDWR
#define FAT_RDONLY          UO_RDONLY 
#define FAT_CREATE          UO_CREATE
#define FAT_APPEND          UO_APPEND


#define FAT_DEVICE          fat_uffs

extern const struct fat_driver fat_uffs;


#endif
