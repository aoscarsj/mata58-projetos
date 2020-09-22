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
enum points_of_error{UNKNOWN,WHILE_ENTERING_MAIN,WHILE_MAKING_DIR,
                     WHILE_OPENING_DIR,WHILE_READING_DIR};
// int errorTreeCopy(int dirOrigin, int dirDestiny,
//          char const * nomeDirOrigem, char const * nomeDirDestino);
int errorDirCopy(DIR * dirOrigin,
                 char * dirOriginPath, char * dirDestinyPath, enum points_of_error PoE);
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
// int errorTreeCopy(int dirOrigin, int dirDestiny,
//          char const * nomeDirOrigem, char const * nomeDirDestino);
int errorDirCopy(DIR * dirOrigin,
                 char * dirOriginPath, char * dirDestinyPath, enum points_of_error
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

int errorDirCopy(DIR * dirOrigin, char * dirOriginPath,
                 char * dirDestinyPath, enum points_of_error PoE) {

/*
    A função "error" recebe as informações sobre o estado das variáveis re-
    ferentes aos arquivos, avalia o valor da variável global errno e imprime
    uma mensagem de error adequada, fechando os file descriptors e encerrando
    o programa se necessário.
*/
// enum points_of_error{UNKNOWN,WHILE_ENTERING_MAIN,WHILE_MAKING_DIR,
//                      WHILE_OPENING_DIR,WHILE_READING_DIR};
    switch (PoE) {
        case WHILE_MAKING_DIR:
            printf("Não foi possível criar o diretório %s",dirDestinyPath);
            break;
        case WHILE_OPENING_DIR:
            printf("Não foi possível abrir o diretório %s",dirOriginPath);
            break;
        case WHILE_READING_DIR:
            printf("Não foi possível ler o diretório %s", dirOriginPath);
            break;
        default:
            printf("Um erro ocorreu", );
            break;
    }

    switch (errno) {
        case EPERM:
            printf(": operação não permitida ao usuário.\n");
            break;

        case ENOENT:
            printf(": o diretório não existe.\n");
            break;

        case EINTR:
            printf(": chamada de sistema interrompida.\n");
            break;

        case EBADF:
            printf(": seu descritor é inadequado.\n");
            break;

        case ENOMEM:
            printf(": não há memória disponível.\n");
            break;

        case EACCES:
            printf(": o usuário não tem acesso a esse diretório.\n");
            break;

        case EBUSY:
            printf(": dispositivo ou recurso estão ocupados.\n");
            break;

        case EEXIST:
            printf(": o diretório já existe.\n");
            break;

        case ENOTDIR:
            printf(": %s não é um diretório.\n", dirOriginPath);
            break;

        case EMFILE:
            printf(": o limite de arquivos e diretórios abertos ");
            printf("para esse processo foi atingido.\n");
            break;

        case ENFILE:
            // nunca ocorre no GNU, mas está definido devido ao POSIX
            printf(": o limite de arquivos e diretórios abertos no sistema ");
            printf("foi atingido.\n");
            break;

        case ETXTBSY:
            printf(": o diretório está sendo utilizado por outro programa.");
            break;

        case ENOSPC:
            printf(": não há espaço livre no dispositivo de armazenamento.\n");
            break;

        case EROFS:
            printf(": o sistema de arquivos é de somente leitura.\n");
            break;

        case ENAMETOOLONG:
            printf(": o nome do diretório é muito longo.\n");
            break;

        case ENOTEMPTY:
            printf(": o diretório não está vazio\n");
            break;

        case ENOTSUP:
            printf(": parâmetros não suportados.\n");

        case ENOTENOUGHARGS:
            // ENOTENOUGHARGS foi definido como error 256, e significa que não foi passado
            // o número correto de argumentos para o programa
            printf(": o programa foi invocado incorretamente. ");
            printf("O programa deve ser invocado da seguinte forma:\n\n");
            printf("treecopy <diretorioorigem> <diretoriodestino>\n\n");
            printf("Nada foi copiado. ");
            break;
            
        default:
            perrorr("");
            printf("\n");
            break;
    }
//
    printf("O programa será encerrado.\n");
    if (dirOrigin != NULL) {
        close(dirOrigin);
    }

    return -1;
}
