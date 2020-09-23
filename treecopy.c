/*Esse programa pode ser executado após compilado da seguinte forma:

treecopy <diretorioorigem> <diretoriodestino>

O programa tentará criar uma cópia idêntica do diretório <diretorioorigem> com o
nome <diretoriodestino>. Esse diretório criado terá permissões 777.

O programa pode falhar por diversos motivos, em alguns casos nada sendo
copiado e em outros a cópia irá falhar no meio da execução.

Após completar a cópia, o programa imprime em stdout uma mensagem contendo
o nome do <diretorioorigem>, o nome de <diretoriodestino>, a quantidade de
diretórios copiados, de arquivos copiados e a quantidade de bytes total.

Retorna 0 se a cópia foi executada perfeitamente, ou -1 caso
algum erro fatal que impediu a conclusão da cópia tenha ocorrido.
*/

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

#define DT_REG 8
#define DT_DIR 4
#define ENOTFILEORDIR 254
#define ENOTENOUGHARGS 255
#define MAXFILEPERMS 0777
enum points_of_error{UNKNOWN,WHILE_ENTERING_MAIN,WHILE_MAKING_DIR,
                     WHILE_OPENING_DIR,WHILE_READING_DIR,NOT_FILE_OR_DIR};
unsigned long long totalBytes = 0;
unsigned int totalFiles = 0, totalDirs = 0;


int errorDirCopy(DIR * dirOrigin, char * dirOriginPath,
                 char * dirCopyPath, enum points_of_error PoE);
int treeCopy(char *originalPath, char *copyPath);

int main(int argc, char *argv[]){
    int returnvalue = 0;
    if (argc != 3){
        errno = ENOTENOUGHARGS;
        return errorDirCopy(NULL,"","",WHILE_ENTERING_MAIN);
        //  os argumentos vazios (NULL,"","") são necessários pois nada foi
        // acessado ainda
    }
    char * pastaOrigem = argv[1];
    char * pastaDestino = argv[2];

    if(!mkdir(pastaDestino,MAXFILEPERMS)){
        returnvalue = treeCopy(pastaOrigem, pastaDestino);
        printf("treecopy: foram copiados %i diretórios, ", totalDirs);
        printf("%i arquivos e %llu bytes ", totalFiles, totalBytes);
        printf("de %s para %s\n", pastaOrigem, pastaDestino);
    }else{
        return errorDirCopy(NULL,pastaOrigem,pastaDestino,WHILE_MAKING_DIR);
    }

    return returnvalue;
}

int errorDirCopy(DIR * dirOrigin, char * dirOriginPath,
                 char * dirCopyPath, enum points_of_error PoE){
    switch (PoE) { // Prefixo que indica onde o erro ocorreu
        case WHILE_MAKING_DIR:
            printf("Não foi possível criar o diretório %s",dirCopyPath);
            break;
        case WHILE_OPENING_DIR:
            printf("Não foi possível abrir o diretório %s",dirOriginPath);
            break;
        case WHILE_READING_DIR:
            printf("Não foi possível ler o diretório %s", dirOriginPath);
            break;
        case NOT_FILE_OR_DIR:
            printf("Não foi possível copiar o arquivo/diretório %s", dirOriginPath);
        default:
            printf("Um erro ocorreu");
            break;
    }

    switch (errno) { // sufixo que indica qual foi o tipo de erro
        case ENOTFILEORDIR:
            printf(": não é um arquivo ou diretório.\n");
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
            // ENOTENOUGHARGS foi definido como error 255, e significa que não foi passado
            // o número correto de argumentos para o programa
            printf(": o programa foi invocado incorretamente. ");
            printf("O programa deve ser invocado da seguinte forma:\n\n");
            printf("treecopy <diretorioorigem> <diretoriodestino>\n\n");
            printf("Nada foi copiado. ");
            break;

        default:
            perror("");
            printf("\n");
            break;
    }
    printf("O programa será encerrado.\n");
    if (dirOrigin != NULL) {
        closedir(dirOrigin);
    }
    return -1;
}

int treeCopy(char *originalPath, char *copyPath){
    /*
    *  A função trecopy recursivamente cria uma copia do diretorio de nome
    * no argumento originalPath com o nome copyPath. Caso o diretório contenha
    * algo que não é arquivo ou diretório comum, a operação é abortada.
    *
    * Retorna 0 caso a cópia tenha sido um sucesso, e -1 se ocorreu algum erro
    * que impediu a conclusão da cópia.
    */

    DIR *origem = opendir(originalPath);
    if(origem){
        struct dirent *child = readdir(origem);
        while( child != NULL){
            // o loop percorre todo o conteúdo do diretório até que
            // readdir(origem) retorne NULL


            char newPathOriginal[PATH_MAX];
            char newPathCopy[PATH_MAX];

            strcpy(newPathOriginal, originalPath);
            strcat(newPathOriginal, "/");
            strcat(newPathOriginal, child->d_name);


            strcpy(newPathCopy, copyPath);
            strcat(newPathCopy, "/");
            strcat(newPathCopy, child->d_name);

            if (strcmp(child->d_name, ".") && strcmp(child->d_name, "..")){
                // filtra os diretórios relativos . e ..
                if(child->d_type == DT_REG){
                    // DT_REG -> arquivo
                    totalFiles++;
                    int copiedBytes = fileCopy(newPathOriginal, newPathCopy);
                    if (copiedBytes == -1) {
                        return -1;
                    }else{
                        totalBytes += copiedBytes;
                    }
                }else if(child->d_type == DT_DIR){
                    // DT_DIR -> diretório
                    if(mkdir(newPathCopy,MAXFILEPERMS)){
                        return errorDirCopy(NULL,newPathOriginal,newPathCopy,WHILE_MAKING_DIR);
                    }
                    int successful = treeCopy(newPathOriginal, newPathCopy);
                    totalDirs++;
                    if (!successful)
                        return -1;
                }else{
                    // nem arquivo nem diretório (não pode copiar)
                    return errorDirCopy(origem,originalPath,copyPath,NOT_FILE_OR_DIR);
                }

            }
            child = readdir(origem);
        }
        if (errno == EBADF) {
            return errorDirCopy(origem,originalPath,copyPath,WHILE_READING_DIR);
        }
    }else{
        return errorDirCopy(NULL,originalPath,copyPath,WHILE_OPENING_DIR);
    }

    closedir(origem);

    return 0;
}
