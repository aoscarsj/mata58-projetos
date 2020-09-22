/*Esse programa pode ser executado após compilado da seguinte forma:

fileCopy arquivoOrigem arquivoDestino

O programa tentará criar uma cópia idêntica de arquivoOrigem com o nome
arquivoDestino. Esse arquivo criado terá permissões 777.

No processo de cópia, um buffer de 4096 bytes é utilizado, garantindo
baixo uso de memória pelo programa. Embora haja uma chamada de sistema
[sendfile()] que execute essa operação, esse programa tem a vantagem de
melhor sinalização e manipulação de erros.

O programa pode falhar por diversos motivos, em alguns casos nada sendo
copiado e em outros a cópia irá falhar no meio da execução.

Após completar a cópia, o programa imprime em stdout uma mensagem contendo
o nome do arquivoOrigem, o nome de arquivoDestino e o número de bytes co-
piados.

Retorna 0 se a cópia foi executada perfeitamente, ou -1 caso
algum erro fatal que impediu a conclusão da cópia tenha ocorrido.
*/

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
    /*
        A função "erro" recebe as informações sobre o estado das variáveis re-
        ferentes aos arquivos, avalia o valor da variável global errno e imprime
        uma mensagem de erro adequada, fechando os file descriptors e encerrando
        o programa se necessário. Alguns erros são apenas notificados, mas não
        causam encerramento do programa.

        Sempre retorna -1.
    */

    bool erroFatal = true;
    // caso seja atribuído "false" o erro será ignorado, porém ainda assim
    // será sinalizado

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
            erroFatal = false;
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
            erroFatal = false;
            break;

        case ENOTENOUGHARGS:
            // ENOTENOUGHARGS foi definido como erro 256, e significa que não foi passado
            // o número correto de argumentos para o programa
            printf("Erro: o programa foi invocado incorretamente. ");
            printf("O programa deve ser invocado da seguinte forma:\n\n");
            printf("filecopy ArquivoOrigem ArquivoDestino\n\n");
            printf("Nenhum arquivo foi copiado. ");
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
            erroFatal = false;
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
            printf("\n");
            break;
    }

    if (erroFatal){
        printf("O programa será encerrado.\n");
        if (arquivoOrigem >= 0) {
            close(arquivoOrigem);
        }
        if (arquivoDestino >= 0){
            close(arquivoDestino);
        }
    }

    return -1;
}





int fileCopy(const char * nomeOriginal, const char * nomeCopia){
    /*
    A função fileCopy realiza o trabalho de copiar o arquivo nomeOriginal para
    o novo arquivo nomeCopia.

    Retorna 0 caso a cópia tenha sido executada com sucesso, e retorna -1 caso
    tenha acontecido algum erro.
    */

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

    o código 0777 representa os níveis de permissao do arquivo a ser criado
    em octal. 0777 é o maior nível de permissão, onde tanto o usuário, usuários
    do mesmo grupo e quaisquer outros podem ler, escrever e executar o arquivo.
    */
    if((arquivoDestino = open(nomeCopia, O_WRONLY | O_CREAT | O_EXCL, 0777)) == -1){
        return erro(arquivoOrigem, arquivoDestino,nomeOriginal,nomeCopia);
    }


    // é criado um ponteiro auxiliar para que não se passe buffer diretamente
    // para a função write()

    int nBytesLidos = 0;
    int nBytesEscritos;
    // loop que realiza a escrita e a contagem dos bytes escritos
    while (nBytesLidos = read(arquivoOrigem, buffer, sizeof buffer), nBytesLidos > 0){
        while ((nBytesEscritos = write(arquivoDestino, buffer, nBytesLidos)) == 0 && errno == EINTR);
        // tenta ler várias vezes caso o erro seja EINTR

        if (errno != EINTR || nBytesEscritos != nBytesLidos)
            return erro(arquivoOrigem, arquivoDestino, nomeOriginal, nomeCopia);
            // ignora o erro EINTR, pois a leitura pode ser tentada novamente
            // sem perdas


    }

    printf("Foram copiados %d bytes ", nBytesEscritosTotal);
    printf("do arquivo %s para o arquivo %s\n\n", nomeOriginal, nomeCopia);

    // rotina de encerramento, fechando os arquivos
    if (nBytesLidos == 0){
        if (close(arquivoDestino) < 0){
            return erro(arquivoOrigem, arquivoDestino,nomeOriginal,nomeCopia);
        }
        if (close(arquivoOrigem) < 0){
            return erro(arquivoOrigem, arquivoDestino,nomeOriginal,nomeCopia);
        }

        if (nBytesLidos == 0)
            return 0;

        return erro(arquivoOrigem, arquivoDestino,nomeOriginal,nomeCopia);
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
