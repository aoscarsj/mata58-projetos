#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

void treeCopy(const char *dirOrigem, const char *dirDestino) {
    DIR *dir;
    DIR *dirDest;
    int pastaCriada;

    //abrindo pasta original
    
    if (!(dir = opendir(dirOrigem))) {
        fprintf(stderr, "Arquivo ou diretório não existe: %s: %s\n", dirOrigem, strerror(errno));
        return;
    }

    // criando nova pasta

    pastaCriada = mkdir(dirDestino,0660);

    if(pastaCriada){
        printf("Não foi possível criar o diretório!\n");
        return;
    }

    //abrindo nova pasta

    if (!(dirDest = opendir(dirDestino))) {
        fprintf(stderr, "Não foi possivel abrir o arquivo: %s: %s\n", dirDestino, strerror(errno));
        return;
    }


    closedir(dir);
    closedir(dirDest);
   
    
}

int main(void) {
    
}