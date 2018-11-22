// SPDX-License-Identifier: GPL-2.0
/*
 *  linux/fs/minix/file.c
 *
 *  Copyright (C) 1991, 1992 Linus Torvalds
 *
 *  minix regular file handling primitives
 */

#include "minix.h"

/*
 * We have mostly NULLs here: the current defaults are OK for
 * the minix filesystem.
 */
 static ssize_t mitm_read_iter (struct kiocb *kio, struct iov_iter *iov);
 static ssize_t mitm_write_iter (struct kiocb *kio, struct iov_iter *iov);

const struct file_operations minix_file_operations = {
	.llseek				= generic_file_llseek,
	.read_iter		= mitm_read_iter,
	.write_iter		= mitm_write_iter,
	// .read 				= mitm_read,
	// .write				= mitm_write,
	.mmap					= generic_file_mmap,
	.fsync				= generic_file_fsync,
	.splice_read	= generic_file_splice_read,
};

// static int mitm_read (struct inode * inode, struct file * filp, char * buf, int count)
// {
// 	int retorno;
// 	printk("Minixmodule: file.c mitm_read");
//   retorno = generic_file_read(inode,filp,buf,count)
// 	return retorno;
// }
//
// static ssize_t mitm_write (struct file *file, const char __user *buf, size_t count, loff_t *ppos)
// {
// 	ssize_t retorno;
// 	printk("Minixmodule: file.c mitm_write");
// 	retorno = generic_file_write(file,buf,count,ppos);
//   return retorno;
// }

static ssize_t mitm_read_iter (struct kiocb *kio, struct iov_iter *iov)
{
	ssize_t retorno;
	/* Descobrir como pegar os dados da kiocb e da iov_iter para assim descriptografar os dados */
	printk("Minixmodule: file.c mitm_read_iter");
	retorno = generic_file_read_iter(kio,iov);
	return (retorno);
}

static ssize_t mitm_write_iter (struct kiocb *kio, struct iov_iter *iov)
{
	ssize_t retorno;
	/* Descobrir como pegar os dados da kiocb e da iov_iter para assim encriptografar os dados */
	printk("Minixmodule: file.c mitm_write_iter");
  retorno =	generic_file_write_iter(kio,iov);
	return(retorno);
}

static int minix_setattr(struct dentry *dentry, struct iattr *attr)
{
	struct inode *inode = d_inode(dentry);
	int error;

	error = setattr_prepare(dentry, attr);
	if (error)
		return error;

	if ((attr->ia_valid & ATTR_SIZE) &&
	    attr->ia_size != i_size_read(inode)) {
		error = inode_newsize_ok(inode, attr->ia_size);
		if (error)
			return error;

		truncate_setsize(inode, attr->ia_size);
		minix_truncate(inode);
	}

	setattr_copy(inode, attr);
	mark_inode_dirty(inode);
	return 0;
}

const struct inode_operations minix_file_inode_operations = {
	.setattr	= minix_setattr,
	.getattr	= minix_getattr,
};
