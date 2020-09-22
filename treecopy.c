#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <dirent.h>
#include "arquivo.c"

#define ENOTENOUGHARGS 256
int createDir(char *path);
unsigned long long bytesTotal = 0;
unsigned int totalFiles = 0, totalDirs = 0;
int list(char *originalPath, char *destinyPath){
    DIR *origem = opendir(originalPath);
    
    if(origem){
        // abriu o diretório de origem
        struct dirent *child = readdir(origem);
        // printf("Path: %s\n", originalPath);
        while( child != NULL){
            
            // enquanto o diretório tiver arquivo ou pasta, continue executando.
            
            if(child->d_type == 8){
                totalFiles++;
                char originalPathChild[500];
                char destinyPathChild[500];
                strcpy(originalPathChild, originalPath);
                strcat(originalPathChild, "/");
                strcat(originalPathChild, child->d_name);

                strcpy(destinyPathChild, destinyPath);
                strcat(destinyPathChild, "/");
                strcat(destinyPathChild, child->d_name);
                
                bytesTotal += fileCopy(originalPathChild, destinyPathChild);
                
            }else if(strcmp(child->d_name, ".") !=0 && strcmp(child->d_name, "..")  != 0){
                totalDirs++;
                //Não pode ser um arquivo nem a pasta . e nem a pasta .. 
                char newPathOriginal[500];
                char newPathDestiny[500];
                strcpy(newPathOriginal, originalPath);
                strcat(newPathOriginal, "/");
                strcat(newPathOriginal, child->d_name);
                

                strcpy(newPathDestiny, destinyPath);
                strcat(newPathDestiny, "/");
                strcat(newPathDestiny, child->d_name);
                createDir(newPathDestiny);
                // printf("Path: %s\n", newPathOriginal);
                list(newPathOriginal, newPathDestiny);
            }
            child = readdir(origem);
        }
    }

    return 1;
}

int createDir(char *path){
    return mkdir(path, 0777); //deveriamos copiar o modo da pasta original n?
}

int main(int argc, char *argv[]){
    if (argc != 3){
        errno = ENOTENOUGHARGS;
        return erro(-1,-1,"","");
        //  esses argumentos para a função erro() sinalizam que nenhum arquivo
        //  foi aberto.
    }
    char * pastaOrigem = argv[1];
    char * pastaDestino = argv[2];

    if(!createDir(pastaDestino)){
        list(pastaOrigem, pastaDestino);
        printf("treecopy: foram copiados %i diretórios, %i arquivos e %llu bytes de %s para %s\n", totalDirs, totalFiles, bytesTotal, pastaOrigem, pastaDestino);
        // print exemplo: treecopy: foram copiados 10 diretórios, 82 arquivos e 102855763
                        // bytes de SO20201 para SO2020SLS
    }
    return (0);
}