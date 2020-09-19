#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

#define ENOTENOUGHARGS 256

typedef enum{false,true} bool;

//fechar arquivos, com prevencao de erros

int erro(int arquivoOrigem, int arquivoDestino,
         char const * nomeArquivoOrigem, char const * nomeArquivoDestino){
    //site com os tipos de erro -> https://ftp.gnu.org/old-gnu/Manuals/glibc-2.2.3/html_node/libc_17.html

    // ENOTENOUGHARGS foi definido como erro 256, e significa que não foi passado
    // o número correto de argumentos para o programa

    switch (errno) {
        case ENOSPC:
            printf("Erro: não há espaço livre no dispositivo de armazenamento.\n");
            break;

        case ENFILE:
            // nunca ocorre no GNU, mas está definido devido ao POSIX
            printf("Erro: o limite de arquivos abertos no sistema ");
            printf("foi atingido.\n");
            break;

        case EMFILE:
            printf("Erro: o limite de arquivos abertos para esse processo ");
            printf("foi atingido.\n");
            break;

        case ENAMETOOLONG:
            printf("Erro: não foi possível ");
            if (arquivoOrigem < 0){
                printf("abrir o arquivo %s ", nomeArquivoOrigem);
            }else if (arquivoDestino < 0){
                printf("criar o arquivo %s ", nomeArquivoDestino);
            }
            printf("pois o nome do arquivo é muito longo.\n");
            break;

        case EACCES:
            printf("Erro: o usuário não tem acesso a esse diretório.\n");
            break;

        case EROFS:
            printf("Erro: o sistema de arquivos é de somente leitura.\n");
            break;

        case ENOMEM:
            printf("Erro: memória de sistema insuficiente para a operação.\n");
            break;

        case EBUSY:
            printf("Erro: dispositivo ou recurso estão ocupados.\n");
            break;

        case EFBIG:
            printf("Erro: o arquivo %s não pode ser ", nomeArquivoOrigem);
            printf("copiado pois seu tamanho é maior do que o máximo ");
            printf("permitido pelo sistema de arquivos.\n");
            break;

        case EBADF:
            printf("Erro: o descriptor do arquivo é inadequado.\n");
            break;

        case ETXTBSY:
            printf("Erro: um dos arquivos está sendo utilizado no momento ");
            printf("por outro programa.\n");
            break;

        case ENOTENOUGHARGS:
            printf("Erro: o programa foi invocado incorretamente. ");
            printf("O programa deve ser invocado da seguinte forma:\n\n");
            printf("filecopy ArquivoOrigem ArquivoDestino\n\n");
            printf("Nenhum arquivo foi copiado e o programa será encerrado.\n");
            break;

        case EPERM:
            printf("Erro: Operação não permitida ao usuário.\n");
            break;

        case ENOENT:
            printf("Erro: Não foi possível abrir o arquivo %s: ", nomeArquivoOrigem);
            printf("Arquivo ou diretório não existe.\n");
            break;

        case EINTR:
            printf("Erro: chamada de sistema interrompida.\n");
            break;

        case EEXIST:
            printf("Erro: Não foi possível copiar o arquivo ");
            printf("%s para o arquivo %s\n", nomeArquivoOrigem,nomeArquivoDestino);
            printf("pois o arquivo %s já existe.\n", nomeArquivoDestino);
            break;

        case EISDIR:
            printf("Erro: Não foi possível abrir o arquivo ");
            if (arquivoOrigem < 0) {
                printf("%s pois %s ", nomeArquivoOrigem, nomeArquivoOrigem );
            }else if(arquivoDestino < 0){
                printf("%s pois %s ", nomeArquivoDestino, nomeArquivoDestino);
            }
            printf("é um diretório e não um arquivo.\n");
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





int fileCopy(const char * nomeOriginal, const char * nomeCopia){
    int arquivoDestino = -1, arquivoOrigem = -1; // receberão os file descriptors
    char buffer[4096]; // buffer de leitura de 4kb
    int nBytesEscritosTotal = 0;

    // O_RDONLY -> modo somente leitura ao abrir o arquivo
    // valor de retorno -1 sinaliza que um erro ocorreu
    if((arquivoOrigem = open(nomeOriginal, O_RDONLY)) == -1){
        return erro(arquivoOrigem,arquivoDestino,nomeOriginal,nomeCopia);
    }

    /*
    O_WRONLY -> modo somente escrita
    O_CREAT -> criar o arquivo caso não exista
    O_EXCL -> previne a criação caso já exista
     valor de retorno -1 sinaliza que um erro ocorreu
    o numero 0660 representa a permissao do arquivo em hexadecimal, no caso:
      0660 significa que usuarios do mesmo grupo do criador do arquivo podem
      ler e escrever e também qualquer outro tipo de usuario também pode
      ler e escrever
        ----rw-rw-
    */
    if((arquivoDestino = open(nomeCopia, O_WRONLY | O_CREAT | O_EXCL, 0660)) == -1){
        return erro(arquivoOrigem, arquivoDestino,nomeOriginal,nomeCopia);
    }


    // é criado um ponteiro auxiliar para que não se passe buffer diretamente
    // para a função write()

    int nBytesLidos = 0;
    int nBytesEscritos;
    // loop que realiza a escrita e a contagem dos bytes escritos
    while (nBytesLidos = read(arquivoOrigem, buffer, sizeof buffer), nBytesLidos > 0){

        nBytesEscritos = write(arquivoDestino, buffer, nBytesLidos);
        // nem sempre serã lido um buffer inteiro (caso alcance o fim do arquivo)

        if (nBytesEscritos >= 0){
            nBytesEscritosTotal += nBytesEscritos;
        }else if (errno != EINTR){
            // ignora o erro EINTR, pois a leitura pode ser tentada novamente
            // sem perdas
            return erro(arquivoOrigem, arquivoDestino, nomeOriginal, nomeCopia);
        }


    }

    printf("Foram copiados %d bytes ", nBytesEscritosTotal);
    printf("do arquivo %s para o arquivo %s\n\n", nomeOriginal, nomeCopia);

    // rotina de encerramento, fechando os arquivos
    if (nBytesLidos == 0){
        if (close(arquivoDestino) < 0){
            arquivoDestino = -1;
            return erro(arquivoOrigem, arquivoDestino,nomeOriginal,nomeCopia);
        }
        close(arquivoOrigem);

        return 0;
    }
}

int main(int argc, char const *argv[]){
    if (argc != 3){
        errno = ENOTENOUGHARGS;
        return erro(-1,-1,"","");
        //  esses argumentos para a função erro() sinalizam que nenhum arquivo
        //  foi aberto.
    }

    char const * arquivoOrigem = argv[1];
    char const * arquivoDestino = argv[2];


    return fileCopy(arquivoOrigem,arquivoDestino);

}
