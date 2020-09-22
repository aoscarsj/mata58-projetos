#include <errno.h>
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <dirent.h>
#include "arquivo.c"
#define FILE 8
#define ENOTENOUGHARGS 256
#define MAXFILEPERMS 0777

// int errorTreeCopy(int dirOrigin, int dirDestiny,
//          char const * nomeDirOrigem, char const * nomeDirDestino);
int errorDirCopy(DIR * dirOrigin,
                 char * dirOriginPath, char * dirDestinyPath);
int errorFileCopy();
unsigned long long totalBytes = 0;
unsigned int totalFiles = 0, totalDirs = 0;
int treeCopy(char *originalPath, char *destinyPath);
/*
    Função utilizada para realizar a copia recursiva do diretorio selecionado para outro
*/

int main(int argc, char *argv[]){
    if (argc != 3){
        errno = ENOTENOUGHARGS;
        return errorDirCopy(NULL,"","");
        //  esses argumentos para a função error() sinalizam que nenhum arquivo
        //  foi aberto.
    }
    char * pastaOrigem = argv[1];
    char * pastaDestino = argv[2];

    if(!mkdir(pastaDestino,MAXFILEPERMS)){
        treeCopy(pastaOrigem, pastaDestino);
        printf("treecopy: foram copiados %i diretórios, %i arquivos e %llu bytes de %s para %s\n", totalDirs, totalFiles, totalBytes, pastaOrigem, pastaDestino);

    }else{
        errorTreeCopy(NULL,pastaOrigem,pastaDestino);
    }
    return 0;
}
int errorDirCopy(DIR * dirOrigin,
                 char * dirOriginPath, char * dirDestinyPath){
    return -1;
}
int treeCopy(char *originalPath, char *destinyPath){
    /*
    *
    */
    DIR *origem = opendir(originalPath);
    if(origem){
        // abriu o diretório de origem


        struct dirent *child = readdir(origem);
        while( child != NULL){

            // enquanto o diretório tiver arquivo ou pasta, continue executando.

                char newPathOriginal[PATH_MAX];
                char newPathDestiny[PATH_MAX];
                strcpy(newPathOriginal, originalPath);
                strcat(newPathOriginal, "/");
                strcat(newPathOriginal, child->d_name);
                printf("%s\n", child->d_name);


                strcpy(newPathDestiny, destinyPath);
                strcat(newPathDestiny, "/");
                strcat(newPathDestiny, child->d_name);

            if(child->d_type == FILE){
                totalFiles++;
                totalBytes += fileCopy(newPathOriginal, newPathDestiny);
            }else if(strcmp(child->d_name, ".") !=0 && strcmp(child->d_name, "..")  != 0){
                totalDirs++;
                //Não pode ser um arquivo nem a pasta . e nem a pasta ..
                if(!mkdir(newPathDestiny,MAXFILEPERMS))
                    errorDirCopy(NULL,originalPath,destinyPath);

                treeCopy(newPathOriginal, newPathDestiny);
            }
            child = readdir(origem);
        }
        if (errno == EBADF)
            errorDirCopy(origem,originalPath,destinyPath);
    }else{
        errorDirCopy(NULL,originalPath,destinyPath);
    }

    return 1;
}


// int errorTreeCopy(int dirOrigin, int dirDestiny,
//          char const * nomeDirOrigem, char const * nomeDirDestino){
//     /*
//         A função "error" recebe as informações sobre o estado das variáveis re-
//         ferentes aos arquivos, avalia o valor da variável global errno e imprime
//         uma mensagem de error adequada, fechando os file descriptors e encerrando
//         o programa se necessário. Alguns errors são apenas notificados, mas não
//         causam encerramento do programa.
//     */
//
//     bool errorFatal = true;
//     // caso seja atribuído "false" o error será ignorado, porém ainda assim
//     // será sinalizado
//
//     switch (errno) {
//         case ENOSPC:
//             printf("Erro: não há espaço livre no dispositivo de armazenamento.\n");
//             break;
//
//         case ENFILE:
//             // nunca ocorre no GNU, mas está definido devido ao POSIX
//             printf("Erro: o limite de arquivos abertos no sistema ");
//             printf("foi atingido.\n");
//             break;
//
//         case EMFILE:
//             printf("Erro: o limite de arquivos abertos para esse processo ");
//             printf("foi atingido.\n");
//             break;
//
//         case ENAMETOOLONG:
//             printf("Erro: não foi possível ");
//             if (dirOrigin < 0){
//                 printf("abrir o diretório/arquivo %s ", nomeDirOrigem);
//             }else if (dirDestiny < 0){
//                 printf("criar o diretório/arquivo %s ", nomeDirDestino);
//             }
//             printf("pois o nome do diretório é muito longo.\n");
//             break;
//
//         case EACCES:
//             printf("Erro: o usuário não tem acesso a esse arquivo/diretório.\n");
//             break;
//
//         case EROFS:
//             printf("Erro: o sistema de arquivos é de somente leitura.\n");
//             break;
//
//         case ENOMEM:
//             printf("Erro: memória de sistema insuficiente para a operação.\n");
//             break;
//
//         case EBUSY:
//             printf("Erro: dispositivo ou recurso estão ocupados.\n");dirDestino
//             break;
//
//         case EFBIG:
//             printf("Erro: o diretório/arquivo %s não pode ser ", nomeDirOrigem);
//             printf("copiado pois seu tamanho é maior do que o máximo ");
//             printf("permitido pelo sistema de arquivos.\n");
//             break;
//
//         case EBADF:
//             printf("Erro: o descriptor do diretório/arquivo é inadequado.\n");
//             break;
//
//         case ETXTBSY:
//             printf("Erro: um dos arquivos/diretórios está sendo utilizado no momento ");
//             printf("por outro programa.\n");
//             break;
//
//         case ENOTENOUGHARGS:
//             // ENOTENOUGHARGS foi definido como error 256, e significa que não foi passado
//             // o número correto de argumentos para o programa
//             printf("Erro: o programa foi invocado incorretamente. ");
//             printf("O programa deve ser invocado da seguinte forma:\n\n");
//             printf("treecopy <diretorioorigem> <diretoriodestino>\n\n");
//             printf("Nada foi copiado. ");
//             break;
//
//         case EPERM:
//             printf("Erro: Operação não permitida ao usuário.\n");
//             break;
//
//         case ENOENT:
//             printf("Erro: Não foi possível abrir o diretório/arquivo %s: ", nomeDirOrigem);
//             printf("Arquivo ou diretório não existe.\n");
//             break;
//
//         case EINTR:
//             printf("Erro: chamada de sistema interrormpida.\n");
//             break;
//
//         case EEXIST:
//             printf("Erro: Não foi possível copiar o diretório/arquivo ");
//             printf("%s para o diretório/arquivo %s\n", nomeDirOrigem,nomeDirDestino);
//             printf("pois o diretório/arquivo %s já existe.\n", nomeDirDestino);
//             break;
//
//         case EISDIR:
//             printf("Erro: Não foi possível abrir o diretório ");
//             if (dirOrigin < 0) {
//                 printf("%s pois %s ", nomeDirOrigem, nomeDirOrigem );
//             }else if(dirDestiny < 0){
//                 printf("%s pois %s ", nomeDirDestino, nomeDirDestino);
//             }
//             printf("é um diretório e não um arquivo.\n");
//             break;
//
//         default:
//             perrorr("Um error ocorreu");
//             printf("\n");
//             break;
//     }
//
//     if (errorFatal){
//         printf("O programa será encerrado.\n");
//         if (dirOrigin >= 0) {
//             close(dirOrigin);
//         }
//         if (dirDestiny >= 0){
//             close(dirDestiny);
//         }
//     }
//
//     return -1;
// }
