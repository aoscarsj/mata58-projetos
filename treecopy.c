#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <dirent.h>
#include "arquivo.c"
int createDir(char *path);
unsigned long long bytesTotal = 0;
int list(char *originalPath, char *destinyPath){
    DIR *origem = opendir(originalPath);
    
    if(origem){
        // abriu o diretório de origem
        struct dirent *child = readdir(origem);
        printf("Path: %s\n", originalPath);
        while( child != NULL){
            
            // enquanto o diretório tiver arquivo ou pasta, continue executando.
            
            if(child->d_type == 8){
                char originalPathChild[500];
                char destinyPathChild[500];
                strcpy(originalPathChild, originalPath);
                strcat(originalPathChild, "/");
                strcat(originalPathChild, child->d_name);

                strcpy(destinyPathChild, destinyPath);
                strcat(destinyPathChild, "/");
                strcat(destinyPathChild, child->d_name);
                
                fileCopy(originalPathChild, destinyPathChild);

            }else if(strcmp(child->d_name, ".") !=0 
                && strcmp(child->d_name, "..")  != 0){
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
                printf("Path: %s\n", newPathOriginal);
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

int main(){
    if(!createDir("pastacopy")){
        list("pasta", "pastacopy");
    }
    return (0);
}