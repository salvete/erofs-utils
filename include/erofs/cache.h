/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 */
/*
 * Copyright (C) 2018 HUAWEI, Inc.
 *             http://www.huawei.com/
 * Created by Miao Xie <miaoxie@huawei.com>
 * with heavy changes by Gao Xiang <xiang@kernel.org>
 */
#ifndef __EROFS_CACHE_H
#define __EROFS_CACHE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "internal.h"

struct erofs_buffer_head;
struct erofs_buffer_block;

#define DATA		0
#define META		1
/* including inline xattrs, extent */
#define INODE		2
/* directory data */
#define DIRA		3
/* shared xattrs */
#define XATTR		4
/* device table */
#define DEVT		5

struct erofs_bhops {
	int (*flush)(struct erofs_buffer_head *bh);
};

struct erofs_buffer_head {
	struct list_head list;
	struct erofs_buffer_block *block;

	erofs_off_t off;
	const struct erofs_bhops *op;

	void *fsprivate;
};

struct erofs_buffer_block {
	struct list_head list;
	struct list_head mapped_list;

	erofs_blk_t blkaddr;
	int type;

	struct erofs_buffer_head buffers;
};

const int get_alignsize(int type, int *type_ret);

extern const struct erofs_bhops erofs_drop_directly_bhops;
extern const struct erofs_bhops erofs_skip_write_bhops;

erofs_off_t erofs_btell(struct erofs_buffer_head *bh, bool end);

static inline int erofs_bh_flush_generic_end(struct erofs_buffer_head *bh)
{
	list_del(&bh->list);
	free(bh);
	return 0;
}

void erofs_buffer_init(struct erofs_sb_info *sbi, erofs_blk_t startblk);
int erofs_bh_balloon(struct erofs_buffer_head *bh, erofs_off_t incr);

struct erofs_buffer_head *erofs_balloc(int type, erofs_off_t size,
				       unsigned int required_ext,
				       unsigned int inline_ext);
struct erofs_buffer_head *erofs_battach(struct erofs_buffer_head *bh,
					int type, unsigned int size);

erofs_blk_t erofs_mapbh(struct erofs_buffer_block *bb);
int erofs_bflush(struct erofs_buffer_block *bb);

void erofs_bdrop(struct erofs_buffer_head *bh, bool tryrevoke);
erofs_blk_t erofs_total_metablocks(void);

#ifdef __cplusplus
}
#endif

#endif
