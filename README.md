# Projeto-2-SO-B

### Grupo
~~~
Bruno Kitaka - 16156341

Paulo Figueiredo - 16043028

Rafael Fioramonte - 16032708

Raíssa Davinha - 15032006

Vinícius Trevisan - 16011231
~~~

<<<<<<< HEAD
### 1. Introdução
Este projeto deverá permitir ao aluno familiarizar-se com os detalhes de implementação de um sistema de arquivos minix cifrado que faz uso da API criptográfica do kernel Linux. Espera-se que ao final do projeto, cada aluno seja capaz de implementar, compilar, instalar e testar um novo módulo de kernel que permita ao usuário montar um sistema de arquivos minix em seu sistema Linux e armazenar arquivos de forma cifrada.

### 2. Descrição do projeto
O projeto consiste em modificar o módulo de kernel que implementa o sistema de arquivos minix em sistemas operacionais Linux, de forma que os arquivos dos usuários sejam armazenados demaneira cifrada. 

Ao carregar o módulo de kernel modificado, deve-se informar no parâmetro key a chave simétrica que será usada para cifrar e decifrar o conteúdo dos arquivos. A chave simétrica corresponde a uma string representada em hexadecimal (cada byte corresponde a dois dígitos hexa). A carga do módulo deve ser executada como no exemplo a seguir:
*insmod minix.ko key=”0123456789ABCDEF”*

Para testar o funcionamento do módulo modificado, deve ser realizada a carga do módulo e criada uma nova partição em seu sistema que será formatada usando-se o sistema de arquivos minix modificado através do comando mkfs.minix. Todos os arquivos armazenados nesta partição deverão ser cifrados pelo módulo minix no momento de sua criação ou atualização utilizando o algoritmo AES com a chave fornecida durante a carga do módulo.

Durante a leitura de um arquivo armazenado nesta partição, o conteúdo do arquivo deve ser decifrado pelo módulo minix utilizando o algoritmo AES em modo ECB com a chave fornecida durante a carga do módulo. Repare que o processo de cifragem dos arquivos será transparente para os programas em espaço de usuário. Isso significa que ao gravar um arquivo no sistema de arquivos utilizando o módulo minix modificado, o conteúdo do arquivo será armazenado cifrado, mas ao ler o arquivo o conteúdo retornado será o conteúdo já decifrado. 

Para verificar se o processo de cifragem dos dados está sendo de fato realizado, o módulo minix modificado deve ser descarregado e o módulo minix original presente no kernel (sem modificações) deve ser carregado. Neste caso, o conteúdo dos arquivos ainda estará acessível aos programas de usuário, mas será diferente do conteúdo originalmente armazenado, pelo fato de não ter sido realizada a decifragem do dados.

### 3. Material complementar
Linux Kernel Crypto API: [https://www.kernel.org/doc/html/v4.12/crypto/index.html](https://www.kernel.org/doc/html/v4.12/crypto/index.html)

The Linux file system: [http://www.tldp.org/LDP/tlk/fs/filesystem.html](http://www.tldp.org/LDP/tlk/fs/filesystem.html)

Anatomy of the Linux file system: [https://www.ibm.com/developerworks/library/l-linuxfilesystem/index.html](https://www.ibm.com/developerworks/library/l-linuxfilesystem/index.html)

Introduction to the Minix File System: [http://ohm.hgesser.de/sp-ss2012/Intro-MinixFS.pdf](http://ohm.hgesser.de/sp-ss2012/Intro-MinixFS.pdf)

### 4. Resultado
O projeto deve ser acompanhado de um relatório com as seguintes partes obrigatórias:
- Introdução, indicando o que se pretende com o experimento;
- Detalhes de projeto do módulo de kernel desenvolvido, detalhando através de textos e diagramas o funcionamento interno do módulo e dos algoritmos criptográficos utilizados;
- Descrição das modificações realizadas no código-fonte do módulo minix original;
- Descrição dos resultados obtidos, detalhando o processo de compilação, instalação e teste do módulo de kernel desenvolvido, demonstrando as funcionalidades implementadas através de imagens e textos descrevendo o que está sendo testado e os resultados esperados e obtidos;
- Conclusão indicando o que foi aprendido com o experimento.

=======
## Tarefas de Vinicius Trevisan
~~~
29/10 - Estudo do problema, como resolvê-lo e por onde começar.
29/10 - Criação de um arquivo utilizando o sistema minix não modificado baseado no tutorial disponibilizado pelo professor
~~~
>>>>>>> 8f772c4459431f7bef98d9fe447e063bffa98732
