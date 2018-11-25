/* Projeto 2 - Modulo criptografico do kernel - Teste Usuario Read e Write
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
  int tamArquivo;
  char mensagem[5000], mensagem2[5000];
  char caminhoArquivo[500];

  FILE *arquivo;
  clear();
  printf("Teste Minixmodule Projeto 2\n");

  do
  {
    printf(".::MENU::.\n");
    printf("1 - Criar\n");
    printf("2 - Atualizar\n");
    printf("0 - Sair\n");
    printf("Selecione sua opcao: ");
    scanf("%i", &opcao);
    getchar();

    printf("Digite o caminho do arquivo a ser aberto: ");
    scanf("%s", caminhoArquivo);
    getchar();

    switch (opcao)
    {
    case 0:
      break;

    case 1:
      arquivo = fopen(caminhoArquivo, "wb+");

      if (!arquivo)
      {
        perror("Erro ao abrir o arquivo\n");
        return errno;
      }

      printf("Digite o que deseja escrever no arquivo %s: ", caminhoArquivo);
      scanf("%[^\n]%*c", mensagem);
      fwrite(&mensagem, sizeof(char), strlen(mensagem), arquivo); /* Enviando a string para o modulo criptografico */

      break;

    case 2:
      arquivo = fopen(caminhoArquivo, "ab+"); /* abre o arquivo para append (leitura inicio escrita final) */

      if (!arquivo)
      {
        perror("Erro ao abrir o arquivo\n");
        return errno;
      }

      tamArquivo = 0;

      printf("Dado lido do arquivo %s:\n", caminhoArquivo);

      while (fread(&mensagem[tamArquivo], sizeof(char), 1, arquivo) != 0)
      {
        tamArquivo++;
      }

      mensagem[tamArquivo] = '\0';

      printf("%s", mensagem);

      printf("O que deseja escrever no arquivo:\n");

      scanf("%[^\n]%*c", mensagem2);

      fwrite(&mensagem, sizeof(char), strlen(mensagem), arquivo);

      break;

    default:

      printf("Opcao invalida, tente novamente\n");

      break;
    }

    fclose(arquivo);

  } while (opcao != 0);

  printf("Fim do programa de testes\n");

  return 0;
}
