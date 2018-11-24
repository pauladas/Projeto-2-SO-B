/* Projeto 1 - Modulo criptografico do kernel - Teste Usuario Read e Write
 *  Bruno Kitaka        - 16156341
 *  Paulo Figueiredo    - 16043028
 *  Rafael Fioramonte   - 16032708
 *  Raissa Davinha      - 15032006
 *  Vinícius Trevisan   - 16011231
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#define clear() printf("\033[H\033[J")

#define BUFFER_LENGTH (256 / 4) * 5 /* Tamanho maximo da mensagem considerada (pode ser maior, porem serao considerados os BUFFER_LENGTH primeiros bytes) */

int main()
{
  int retorno, opcao;
  int caractere;
  int tamArquivo;
  char mensagem[5000];
  char caminhoArquivo[500];

  int i, j;
  FILE *arquivo;

  clear();
  printf("Teste Minixmodule Projeto 2\n");

  do
  {
    printf(".::MENU::.\n");
    printf("1 - Gravar\n");
    printf("2 - Ler\n");
    printf("0 - Sair\n");
    printf("Selecione sua opcao: ");
    scanf("%i", &opcao);
    getchar();

    if (opcao != 0)
    {
      printf("Digite o caminho do arquivo a ser aberto: ");
      scanf("%s", caminhoArquivo);
      getchar();
      arquivo = fopen(caminhoArquivo, "wb+"); /* Realiza a abertura do arquivo do modulo e salva o file descriptor em arquivo */
      if (!arquivo)
      {
        perror("Erro ao abrir o arquivo\n");
        return errno;
      }

      switch (opcao)
      {
      case 1:
        printf("Digite o que deseja escrever no arquivo %s: ", caminhoArquivo);
        scanf("%[^\n]%*c", mensagem);
        fwrite(&mensagem, sizeof(char), strlen(mensagem), arquivo); /* Enviando a string para o modulo criptografico */
        break;

      case 2:
        tamArquivo = 0;
        printf("Dado lido do arquivo %s:\n", caminhoArquivo);
        while (fread(&mensagem[tamArquivo], sizeof(char), 1, arquivo) != 0)
        {
          tamArquivo++;
        }

        mensagem[tamArquivo] = '\0';

        printf("%s", conteudoArquivo);
        break;

      default:
        printf("Opcao invalida, tente novamente\n");
      }
      fclose(arquivo);
    }
  } while (opcao != 0);

  printf("Fim do programa de testes\n");
  return 0;
}
