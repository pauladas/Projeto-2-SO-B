// SPDX-License-Identifier: GPL-2.0
/*
 *  linux/fs/minix/file.c
 *
 *  Copyright (C) 1991, 1992 Linus Torvalds
 *
 *  minix regular file handling primitives
 */

#include "minix.h"
#include <linux/crypto.h>
#include <linux/vmalloc.h>

/* estruturas para funcao de criptografia */
struct tcrypt_result {
	struct completion completion;
	int err;
};

struct skcipher_def {
	struct scatterlist sg;
	struct crypto_skcipher * tfm;
	struct skcipher_request * req;
	struct tcrypt_result result;
	char * scratchpad;
	char * ciphertext;
	char * ivdata;
	unsigned int encrypt;
};

static struct skcipher_def sk;                          /* estrutura para funcao de encriptar */
static int qtdBlocos;
static char * dados;
static char * resultadoCripto;
static int tamanhoDados;

/*
 * We have mostly NULLs here: the current defaults are OK for
 * the minix filesystem.
 */
 static ssize_t mitm_read_iter (struct kiocb *kio, struct iov_iter *iov);
 static ssize_t mitm_write_iter (struct kiocb *kio, struct iov_iter *iov);
 static void 	  test_skcipher_finish(struct skcipher_def * sk);
 static int 	  test_skcipher_result(struct skcipher_def * sk, int rc);
 static void 	  test_skcipher_callback(struct crypto_async_request *req, int error);
 static int 	  test_skcipher_encrypt(char * plaintext, struct skcipher_def * sk);

const struct file_operations minix_file_operations = {
	.llseek				= generic_file_llseek,
	.read_iter		= mitm_read_iter,
	.write_iter		= mitm_write_iter,
	.mmap					= generic_file_mmap,
	.fsync				= generic_file_fsync,
	.splice_read	= generic_file_splice_read,
};

static ssize_t mitm_read_iter (struct kiocb *kio, struct iov_iter *iov_it)
{
	ssize_t retorno;
	int i;
	/* Descobrir como pegar os dados da kiocb e da iov_iter para assim descriptografar os dados */
	printk("Minixmodule: file.c mitm_read_iter\n");

	printk("Minixmodule: Tam -> %i\n",strlen(iov_it->iov->iov_base));
	tamanhoDados = strlen(iov_it->iov->iov_base);
	if(tamanhoDados>0)
	{
		qtdBlocos = tamanhoDados / 16;
		resultadoCripto = (char *)vmalloc(qtdBlocos*16);
		sk.tfm = NULL;
		sk.req = NULL;
		sk.scratchpad = NULL;
		sk.ciphertext = NULL;
		sk.ivdata = NULL;
		sk.encrypt = 0;                                        /* operacao: 1 para encriptar e 0 para decriptar */
		test_skcipher_encrypt((char*)iov_it->iov->iov_base, &sk);/* chamada de funcao (string para encriptar, chave, estrutura para encriptar) */
		test_skcipher_finish(&sk);                             /* funcao para retirar resultado do scatterlist */
		// printk("iovBase");
		// for(i=0; i< tamanhoDados; i++)
		// {
		// 	printk("%02hhx ",((char*)iov_it->iov->iov_base)[i]);
		// }
		memcpy(iov_it->iov->iov_base, resultadoCripto,qtdBlocos*16);

		printk("Resultado");
		for(i=0; i< tamanhoDados; i++)
		{
			printk("%c ",((char*)iov_it->iov->iov_base)[i]);
		}
		vfree(resultadoCripto);
	}
	printk("Minixmodule: Fim Read\n");
	retorno = generic_file_read_iter(kio,iov_it);
	return (retorno);
}

static ssize_t mitm_write_iter (struct kiocb *kio, struct iov_iter *iov_it)
{
	ssize_t retorno;
	int i;
  struct iovec *modificado = iov_it->iov;
	/* Descobrir como pegar os dados da kiocb e da iov_iter para assim encriptografar os dados */
	printk("Minixmodule: file.c mitm_write_iter\n");

	// Cria um ponteiro para salvar os dados nao encriptados
	dados = (char *)vmalloc((int)(iov_it->count) + 1);
	for(i=0; i<(int)(iov_it->count); i++)
			dados[i] = ((char *)(iov_it->iov->iov_base))[i];
	dados[i] = '\0';
	tamanhoDados = (int)(iov_it->count);
	qtdBlocos = tamanhoDados / 16;
	if(sizeof(tamanhoDados) % 16 != 0) qtdBlocos++;
	resultadoCripto = (char *)vmalloc(qtdBlocos*16);
	/* Inicializacao da funcao de encryptar */
	sk.tfm = NULL;
	sk.req = NULL;
	sk.scratchpad = NULL;
	sk.ciphertext = NULL;
	sk.ivdata = NULL;
	sk.encrypt = 1;                                        /* operacao: 1 para encriptar e 0 para decriptar */
	test_skcipher_encrypt(dados, &sk);/* chamada de funcao (string para encriptar, chave, estrutura para encriptar) */
	test_skcipher_finish(&sk);                             /* funcao para retirar resultado do scatterlist */

	modificado->iov_len = qtdBlocos*16;
	memcpy(iov_it->iov->iov_base, resultadoCripto,qtdBlocos*16);

	iov_it->count =(size_t) qtdBlocos*16;

  /* O write retorna o tamanho dos dados facilitando a encriptacao */
	/* Eh o mais facil -> Ideia: Pegar os dados (ponteiro (char *)(iov_it->iov->iov_base))) e a quantidade de dados (ponteiro (int)(iov_it->iov->iov_len)))
		 Quebrar em blocos, encriptografar e salvar em uma estrutura auxiliar (vmalloc) e copiar o ponteiro para iov_base */
  retorno =	generic_file_write_iter(kio,iov_it);
	vfree(dados);
	vfree(resultadoCripto);
	printk("Minixmodule: Fim Write\n");
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

static void test_skcipher_finish(struct skcipher_def * sk) /* função de desalocação das memorias utilizadas */
{
	if (sk->tfm)
		crypto_free_skcipher(sk->tfm);   /* desaloca o algoritimo de transformacao */
	if (sk->req)
		skcipher_request_free(sk->req);  /* desaloca os dados de requisicao para a encriptacao */

  /*ivdata: vetor de inicializacao, variavel a ser utilizada em conjunto com a chave de encryptação para evitar q a string de saida tenha partes parecidas,
            isso evita q algum terceiro analise algum padrao gerado tornando a encrytacao mais segura, nao eh necessario ter o ivdata para decryptar mas caso for possivel
            ter a chave e o ivdata a encryptacao eh mais rapida
  */
	if (sk->ivdata)
		vfree(sk->ivdata);      /* libebra o vetor de inicializacao */
	if (sk->scratchpad)
		vfree(sk->scratchpad);  /* desaloca o espaco aonde ira a entrada antes de ser passada para a scatterlist */
	if (sk->ciphertext)
		vfree(sk->ciphertext);  /* desaloca o espaco aonde ira a saida da funcao de decriptar */
}

static int test_skcipher_result(struct skcipher_def * sk, int rc) /* testa o retorna da encriptacao */
{
	switch (rc)
  {
		case 0:              /* caso for 0 encriptacao terminou sem erros */
			break;
	  case -EINPROGRESS: /* caso contrario trata o erro retornado */
	  case -EBUSY:
		  rc = wait_for_completion_interruptible(&sk->result.completion);
		  if (!rc && !sk->result.err)
      {
  			reinit_completion(&sk->result.completion);
  			break;
		  }
	  default:
		  printk("MinixModule: skcipher encrypt retornou com %d resultado %d\n", rc, sk->result.err);
		break;
	}

	init_completion(&sk->result.completion); /* caso n tiver erros termina encrytacao */
	return rc;
}

static void test_skcipher_callback(struct crypto_async_request *req, int error) /* funcao de callback, chamada quando o crypto termina */
{
	struct tcrypt_result *result = req->data; /* estrutura result para funcao complete() */
	if (error == -EINPROGRESS)                /* caso a funcao estiver executando ainda, retornar e esperar ate terminar */
		return;
	result->err = error;                      /* atribui erro da funcao do crypto */
	complete(&result->completion);            /* completar a funcao de cryto de acordo com seu erro, caso n tiver termina normalmente */
	printk("MinixModule: Criptografia terminada com sucesso\n");
}

/* argumentos: plaintext -> conteudo a ser encryptado; password -> chave de encryptacao; sk -> estrutura com todas as informacoes necessarias para a encryptacao */
static int test_skcipher_encrypt(char * plaintext, struct skcipher_def * sk) /* funcao "pai" de criptografia */
{
  int j;
	int ret = -EFAULT;                                         /* retorno da funcao */
	if (!sk->tfm)                                              /* aloca variavel de algoritimo de transformacao */
  {
		sk->tfm = crypto_alloc_skcipher("ecb-aes-aesni", 0, 0);  /* definindo o algoritimo aes, ecb */
		if (IS_ERR(sk->tfm))                                     /* caso erro na alocacao do objeto de tranformacao */
    {
			printk("MinixModule: Nao foi possivel alocar o objeto de transformacao\n");
			return PTR_ERR(sk->tfm);
		}
	}
	if (!sk->req)   /* aloca variavel req, req contem todas as informacoes necessarias para fazer uma requisicao de encryptacao */
  {
		sk->req = skcipher_request_alloc(sk->tfm, GFP_KERNEL);
		if (!sk->req) /* caso falha na alocacao impirme mensagem de erro*/
    {
			printk("MinixModule: Nao foi possivel alocar o request\n");
			ret = -ENOMEM;
			return ret;
		}
	}

  /* direciona qual a funcao de callback ao terminar o request de encryptacao */
	skcipher_request_set_callback(sk->req, CRYPTO_TFM_REQ_MAY_BACKLOG,test_skcipher_callback,&sk->result);

	/* atrelando chave ao algoritimo de transformacao AES 256*/
	if (crypto_skcipher_setkey(sk->tfm, keyHexa, KEY_SIZE/8)) {
		printk("MinixModule: Nao foi possivel atrelar a chave ao algoritmo de tranformacao\n");
		ret = -EAGAIN;
		return ret;
	}

	if (!sk->ivdata) { /* aloca iv data aleatorio */
		/* mais info https://en.wikipedia.org/wiki/Initialization_vector */
		sk->ivdata = vmalloc(CIPHER_BLOCK_SIZE);
		if (!sk->ivdata) {
			printk("MinixModule: Nao foi possivel alocar ivDATA\n");
			return ret;
		}
		get_random_bytes(sk->ivdata, CIPHER_BLOCK_SIZE);
	}

	if (!sk->scratchpad) { /* alocar variavel para texto a ser criptografado */
		sk->scratchpad = vmalloc(CIPHER_BLOCK_SIZE);
		if (!sk->scratchpad) {
			printk("MinixModule: Nao foi possivel alocar o Scratchpad\n");
			return ret;
		}
	}

  for(j=0; j< qtdBlocos; j++)
  {
    memcpy(sk->scratchpad,plaintext+CIPHER_BLOCK_SIZE*j,CIPHER_BLOCK_SIZE);
    sg_init_one(&sk->sg, sk->scratchpad, CIPHER_BLOCK_SIZE); //inicializar text na scatterlist
  	skcipher_request_set_crypt(sk->req, &sk->sg, &sk->sg,	CIPHER_BLOCK_SIZE, sk->ivdata); //especifica qual as variaveis para encriptacao
    //request com todos os dados para encriptar, fonte dos dados a ser encriptado, destino dos dados a ser encriptado, tamanho do bloco, ivdata
  	init_completion(&sk->result.completion); //iniciar criptogragia

  	if (sk->encrypt) { //caso a variavel encrypt for 1 encriptar
  		ret = crypto_skcipher_encrypt(sk->req);
  	} else { //caso a variavel encrypt for 0 decriptar
  		ret = crypto_skcipher_decrypt(sk->req);
  	}

  	ret = test_skcipher_result(sk, ret);//testar resultado da encriptacao

  	if (ret)
  		return ret;

    sk->ciphertext = sg_virt(&(sk->sg));//ponteiro da scatterlist para variavel ciphertext
		// EDITAR AQUI
    memcpy(resultadoCripto+CIPHER_BLOCK_SIZE*j,sk->scratchpad,CIPHER_BLOCK_SIZE);
  	sk->ciphertext = NULL;//zera variavel ciphertext para n dar erro na funcao finish
  }

 	return ret;
}
