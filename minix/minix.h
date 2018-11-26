/* SPDX-License-Identifier: GPL-2.0 */
#ifndef FS_MINIX_H
#define FS_MINIX_H

#include <linux/fs.h>
#include <linux/pagemap.h>
#include <linux/minix_fs.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <crypto/internal/skcipher.h>  /* Biblioteca para encriptar e decriptar */


#define INODE_VERSION(inode)	minix_sb(inode->i_sb)->s_version
#define MINIX_V1		0x0001		/* original minix fs */
#define MINIX_V2		0x0002		/* minix V2 fs */
#define MINIX_V3		0x0003		/* minix V3 fs */
#define CIPHER_BLOCK_SIZE 16 		       /* tamanho do bloco para encriptacao */
#define KEY_SIZE 256

extern char *key;                                       /* Armazena a chave em CARACTERES passada como parametro na insercao do modulo -- Durante a conversao, o limite definido foi de 64 caracteres hexa de entrada ou seja, 32 bytes*/
extern char keyHexa[(KEY_SIZE / 8) + 1];                      /* Armazena a chave HEXADECIMAL convertida a partir da chave em CARACTERES */
extern char keyChar[(KEY_SIZE / 4) + 1];                  /* Representacao em caracteres da chave considerada em hexadecimal */
extern int size_of_message;                             /* Guarda o tamanho da entrada quando o usuario grava alguma coisa no arquivo do modulo */
extern int size_of_key;

// struct iov_iter {
// 			 int type;
// 			 size_t iov_offset;
// 			 size_t count;
// 			 union {
// 					 const struct iovec *iov;
// 					 const struct kvec *kvec;
// 					 const struct bio_vec *bvec;
// 					 struct pipe_inode_info *pipe;
// 			 };
// 			 union {
// 					 unsigned long nr_segs;
// 					 struct {
// 							 int idx;
// 							 int start_idx;
// 					 };
// 		 };
// 	 };
//
// 	 struct iovec
// 	 {
// 			 void __user *iov_base;	/* BSD uses caddr_t (1003.1g requires void *) */
// 			 __kernel_size_t iov_len; /* Must be size_t (1003.1g) */
// };
/*
 * minix fs inode data in memory
 */
struct minix_inode_info {
	union {
		__u16 i1_data[16];
		__u32 i2_data[16];
	} u;
	struct inode vfs_inode;
};

/*
 * minix super-block data in memory
 */
struct minix_sb_info {
	unsigned long s_ninodes;
	unsigned long s_nzones;
	unsigned long s_imap_blocks;
	unsigned long s_zmap_blocks;
	unsigned long s_firstdatazone;
	unsigned long s_log_zone_size;
	unsigned long s_max_size;
	int s_dirsize;
	int s_namelen;
	struct buffer_head ** s_imap;
	struct buffer_head ** s_zmap;
	struct buffer_head * s_sbh;
	struct minix_super_block * s_ms;
	unsigned short s_mount_state;
	unsigned short s_version;
};

extern struct inode *minix_iget(struct super_block *, unsigned long);
extern struct minix_inode * minix_V1_raw_inode(struct super_block *, ino_t, struct buffer_head **);
extern struct minix2_inode * minix_V2_raw_inode(struct super_block *, ino_t, struct buffer_head **);
extern struct inode * minix_new_inode(const struct inode *, umode_t, int *);
extern void minix_free_inode(struct inode * inode);
extern unsigned long minix_count_free_inodes(struct super_block *sb);
extern int minix_new_block(struct inode * inode);
extern void minix_free_block(struct inode *inode, unsigned long block);
extern unsigned long minix_count_free_blocks(struct super_block *sb);
extern int minix_getattr(const struct path *, struct kstat *, u32, unsigned int);
extern int minix_prepare_chunk(struct page *page, loff_t pos, unsigned len);

extern void V1_minix_truncate(struct inode *);
extern void V2_minix_truncate(struct inode *);
extern void minix_truncate(struct inode *);
extern void minix_set_inode(struct inode *, dev_t);
extern int V1_minix_get_block(struct inode *, long, struct buffer_head *, int);
extern int V2_minix_get_block(struct inode *, long, struct buffer_head *, int);
extern unsigned V1_minix_blocks(loff_t, struct super_block *);
extern unsigned V2_minix_blocks(loff_t, struct super_block *);

extern struct minix_dir_entry *minix_find_entry(struct dentry*, struct page**);
extern int minix_add_link(struct dentry*, struct inode*);
extern int minix_delete_entry(struct minix_dir_entry*, struct page*);
extern int minix_make_empty(struct inode*, struct inode*);
extern int minix_empty_dir(struct inode*);
extern void minix_set_link(struct minix_dir_entry*, struct page*, struct inode*);
extern struct minix_dir_entry *minix_dotdot(struct inode*, struct page**);
extern ino_t minix_inode_by_name(struct dentry*);

extern const struct inode_operations minix_file_inode_operations;
extern const struct inode_operations minix_dir_inode_operations;
extern const struct file_operations minix_file_operations;
extern const struct file_operations minix_dir_operations;

static inline struct minix_sb_info *minix_sb(struct super_block *sb)
{
	return sb->s_fs_info;
}

static inline struct minix_inode_info *minix_i(struct inode *inode)
{
	return container_of(inode, struct minix_inode_info, vfs_inode);
}

static inline unsigned minix_blocks_needed(unsigned bits, unsigned blocksize)
{
	return DIV_ROUND_UP(bits, blocksize * 8);
}

#if defined(CONFIG_MINIX_FS_NATIVE_ENDIAN) && \
	defined(CONFIG_MINIX_FS_BIG_ENDIAN_16BIT_INDEXED)

#error Minix file system byte order broken

#elif defined(CONFIG_MINIX_FS_NATIVE_ENDIAN)

/*
 * big-endian 32 or 64 bit indexed bitmaps on big-endian system or
 * little-endian bitmaps on little-endian system
 */

#define minix_test_and_set_bit(nr, addr)	\
	__test_and_set_bit((nr), (unsigned long *)(addr))
#define minix_set_bit(nr, addr)		\
	__set_bit((nr), (unsigned long *)(addr))
#define minix_test_and_clear_bit(nr, addr) \
	__test_and_clear_bit((nr), (unsigned long *)(addr))
#define minix_test_bit(nr, addr)		\
	test_bit((nr), (unsigned long *)(addr))
#define minix_find_first_zero_bit(addr, size) \
	find_first_zero_bit((unsigned long *)(addr), (size))

#elif defined(CONFIG_MINIX_FS_BIG_ENDIAN_16BIT_INDEXED)

/*
 * big-endian 16bit indexed bitmaps
 */

static inline int minix_find_first_zero_bit(const void *vaddr, unsigned size)
{
	const unsigned short *p = vaddr, *addr = vaddr;
	unsigned short num;

	if (!size)
		return 0;

	size >>= 4;
	while (*p++ == 0xffff) {
		if (--size == 0)
			return (p - addr) << 4;
	}

	num = *--p;
	return ((p - addr) << 4) + ffz(num);
}

#define minix_test_and_set_bit(nr, addr)	\
	__test_and_set_bit((nr) ^ 16, (unsigned long *)(addr))
#define minix_set_bit(nr, addr)	\
	__set_bit((nr) ^ 16, (unsigned long *)(addr))
#define minix_test_and_clear_bit(nr, addr)	\
	__test_and_clear_bit((nr) ^ 16, (unsigned long *)(addr))

static inline int minix_test_bit(int nr, const void *vaddr)
{
	const unsigned short *p = vaddr;
	return (p[nr >> 4] & (1U << (nr & 15))) != 0;
}

#else

/*
 * little-endian bitmaps
 */

#define minix_test_and_set_bit	__test_and_set_bit_le
#define minix_set_bit		__set_bit_le
#define minix_test_and_clear_bit	__test_and_clear_bit_le
#define minix_test_bit	test_bit_le
#define minix_find_first_zero_bit	find_first_zero_bit_le

#endif

#endif /* FS_MINIX_H */
