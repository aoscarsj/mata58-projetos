#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

#define ENOTENOUGHARGS 256

//fechar arquivos, com prevencao de erros generica

int erro(int arquivoOrigem, int arquivoDestino,
         char const * nomeArquivoOrigem, char const * nomeArquivoDestino){
    //site com os tipos de erro -> https://www.thegeekstuff.com/2010/10/linux-error-codes/

    // ENOTENOUGHARGS foi definido como erro 256, e significa que não foi passado
    // o número correto de argumentos para o programa

    /*
    A INCLUIR


    #define EIO              5      / I/O error /
    #define EEXIST          17      / File exists /
    #define EISDIR          21      / Is a directory /
    #define ETXTBSY         26      / Text file busy /
    #define EBADF            9      / Bad file number /
    #define EROFS           30      / Read-only file system /
    #define ENOMEM          12      / Out of memory /
    #define EBUSY           16      / Device or resource busy /
    #define EFBIG           27      / File too large /
    #define EACCES          13      / Permission denied /
    #define ENAMETOOLONG    36      / File name too long /
    #define ENFILE          23      / File table overflow /
    #define EMFILE          24      / Too many open files /
    #define ENOSPC          28      / No space left on device /
    */
    switch (errno) {
        case ENOTENOUGHARGS:
            perror("Um erro ocorreu");
            printf("Erro: o programa foi invocado incorretamente. ");
            printf("O programa deve ser invocado da seguinte forma:\n\n");
            printf("filecopy ArquivoOrigem ArquivoDestino\n\n");
            printf("Nenhum arquivo foi copiado e o programa será encerrado.\n");
            break;
        case EPERM:
            printf("Operação não permitida\n");
            break;
        case ENOENT:
            printf("Não foi possível abrir o arquivo %s: ", original);
            printf("Arquivo ou diretório não existe.\n");
            break;
        case EINTR:
            printf("Chamada de sistema interrompida.\n");
            break;
        default:
            perror("Um erro ocorreu");
            break;
    }

    if (arquivoOrigem >= 0) {
        close(arquivoOrigem);
    }
    if (arquivoDestino >= 0){
        close(arquivoDestino);
    }

    return -1;
}

//arquivoDestino é o arquivo q sera criado e receberá a copia
//arquivoOrigem é o arquivo q ja existe e vai ser copiado
//buffer é o array de char onde sera armazenado o conteudo lido
//leituraArquivo é o resultado da operacao de leitura
//capturaErro é pra capturar os possiveis erros, mas n ta muito bom ainda
//aux é pra captura do numero de bytes



int fileCopy(const char *original, const char *copia){
    int arquivoDestino, arquivoOrigem;
    char buffer[4096]; // transferir somente 4kb, seguindo a exigencia da implementação
    int capturaErro;
    int leituraArquivo;
    int bytesEscritos = 0;

    //original é o nome do arquivo que ja existe e vai ser copiado
    //segundo parametro é a permissao do mesmo, no caso é somente para leitura: O_RDONLY == read only
    //se retornar -1 é pq aconteceu algum erro

    if((arquivoOrigem = open(original, O_RDONLY)) == -1){
        return erro(arquivoOrigem,arquivoDestino,original,copia);
    }


    //copia é o nome do arquivo q sera criado
    //segundo parametro é a permissao do mesmo, no caso:
            //O_WRONLY == write only == somente escrita
            //O_CREAT == criar o arquivo se n existir
            //O_EXCL == previne a criação se ja existir
    //se retornar o -1 é pq aconteceu algum erro

    /*o numero 0660 representa a permissao do arquivo em hexadecimal, no caso:
      0660 significa que usuarios do mesmo grupo do criador do arquivo pode ler e escrever(read and write) e
      tb qualquer outro tipo de usuario tb pode ler e escrever (read and write)
        ----rw-rw-
    */

    //site para determinar permissoes do arquivo -> https://ss64.com/bash/chmod.html
    //site q pode auxiliar no entendimento da escolha da numeração -> https://stackoverflow.com/questions/18415904/what-does-mode-t-0644-mean/18415935


    if((arquivoDestino = open(copia, O_WRONLY | O_CREAT | O_EXCL, 0660)) == -1){
        printf("Não foi possível copiar o arquivo %s para o arquivo %s, pois o arquivo %s já existe.\n", original,copia,copia);// se o arquivo existir

        return erro(arquivoOrigem, arquivoDestino,original,copia);

    }


    /* leituraArquivo vai receber o arquivo de origem, o array de char (buffer) onde sera armazenado o conteudo lido
       e o numero de bytes do buffer. Ja a outra condicao do while é para parar assim q terminar de ler o arquivo todo
    */

    //é criado um ponteiro auxiliar para estabelecer o array de char, no caso, uma copia de buffer, para quando realizar a escrita

    /*escritaArquivo é onde vai ser armazenado a escrita
      arquivoDestino é o arquivo criado
      ponteiroEscrita é o ponteiro para o buffer, no qual vai se executar a escrita do arquivo
      leituraArquivo é onde estara o arquivo para escrita
    */

    /*no if é onde ocorre de fato a copia do arquivo
      e tb onde é capturado o numero de bytes com o 'bytesEscritos'
    */
    while (leituraArquivo = read(arquivoOrigem, buffer, sizeof buffer), leituraArquivo > 0){
        char *ponteiroEscrita = buffer;
        int escritaArquivo;

            escritaArquivo = write(arquivoDestino, ponteiroEscrita, leituraArquivo);

            if (escritaArquivo >= 0){
                leituraArquivo -= escritaArquivo;
                ponteiroEscrita += escritaArquivo;
                bytesEscritos += escritaArquivo;
                //printf("%d\n ", aux); caso queira ver se o arquivo esta sendo enviado de 4 em 4kb

            }

            //System calls that are interrupted by signals can either abort and return EINTR
            //chamadas de sistema q sao interrompidas podem ser abortadas e retornar EINTR
            else if (errno != EINTR){
                return erro(arquivoOrigem, arquivoDestino,original,copia);
            }

    }

    printf("Foram copiados %d bytes do arquivo %s para o arquivo %s\n", bytesEscritos, original, copia);//  quantidade de nbytes

    //se entrar nesse if, é pq a operação foi concluida com sucesso
    if (leituraArquivo == 0){
        if (close(arquivoDestino) < 0){
            arquivoDestino = -1;
            return erro(arquivoOrigem, arquivoDestino,original,copia);
        }
        close(arquivoOrigem);

        return 0;
    }
}

int main(int argc, char const *argv[]){
    if (argc != 3){
        errno = ENOTENOUGHARGS;
        return erro(-1,-1,"","");
    }

    char const * arquivoOrigem = argv[1];
    char const * arquivoDestino = argv[2];
    //primeiro parametro é o nome do arquivo q ja existe
    //segundo parametro é o nome do arquivo a ser criado e receber a copia de dados

    return fileCopy(arquivoOrigem,arquivoDestino);

}
