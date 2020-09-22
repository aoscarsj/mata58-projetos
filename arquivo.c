#ifndef ARQUIVOMATA58_C
#define ARQUIVOMATA58_C

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

#define ENOTENOUGHARGS 256

typedef enum{false,true} bool;


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
        return -1;
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
        return -1;
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
            nBytesLidos = -1; // registra que um erro ocorreu e não leu
                              // o arquivo corretamente.
            break;
        }


    }

    // rotina de encerramento, fechando os arquivos
    if (close(arquivoDestino) < 0){
        return -1;
    }
    if (close(arquivoOrigem) < 0){
        return -1;
    }

    if (nBytesLidos == 0){
        return nBytesEscritosTotal;
    }

    return -1;
}

#endif /*ARQUIVOMATA58_C*/
