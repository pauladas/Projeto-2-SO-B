// SPDX-License-Identifier: GPL-2.0
#include <linux/buffer_head.h>
#include <linux/slab.h>
#include "minix.h"

enum {DEPTH = 3, DIRECT = 7};	/* Only double indirect */

typedef u16 block_t;	/* 16 bit, host order */

static inline unsigned long block_to_cpu(block_t n)
{
	printk("Minixmodule: itree_v1.c block_to_cpu\n");
	return n;
}

static inline block_t cpu_to_block(unsigned long n)
{
	printk("Minixmodule: itree_v1.c cpu_to_block\n");
	return n;
}

static inline block_t *i_data(struct inode *inode)
{
	printk("Minixmodule: itree_v1.c i_data\n");
	return (block_t *)minix_i(inode)->u.i1_data;
}

static int block_to_path(struct inode * inode, long block, int offsets[DEPTH])
{
	int n = 0;
	printk("Minixmodule: itree_v1.c block_to_path\n");

	if (block < 0) {
		printk("MINIX-fs: block_to_path: block %ld < 0 on dev %pg\n",
			block, inode->i_sb->s_bdev);
	} else if (block >= (minix_sb(inode->i_sb)->s_max_size/BLOCK_SIZE)) {
		if (printk_ratelimit())
			printk("MINIX-fs: block_to_path: "
			       "block %ld too big on dev %pg\n",
				block, inode->i_sb->s_bdev);
	} else if (block < 7) {
		offsets[n++] = block;
	} else if ((block -= 7) < 512) {
		offsets[n++] = 7;
		offsets[n++] = block;
	} else {
		block -= 512;
		offsets[n++] = 8;
		offsets[n++] = block>>9;
		offsets[n++] = block & 511;
	}
	return n;
}

#include "itree_common.c"

int V1_minix_get_block(struct inode * inode, long block,
			struct buffer_head *bh_result, int create)
{
	printk("Minixmodule: itree_v1.c V1_minix_get_block\n");
	return get_block(inode, block, bh_result, create);
}

void V1_minix_truncate(struct inode * inode)
{
	printk("Minixmodule: itree_v1.c V1_minix_truncate\n");
	truncate(inode);
}

unsigned V1_minix_blocks(loff_t size, struct super_block *sb)
{
	printk("Minixmodule: itree_v1.c V1_minix_blocks\n");
	return nblocks(size, sb);
}
