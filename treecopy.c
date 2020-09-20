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

    char Path[256], *EndPtr = Path;
        struct dirent *e;
        strcpy(Path, dirOrigem);                  //Copies the current path to the 'Path' variable.
        EndPtr += strlen(dirOrigem);              //Moves the EndPtr to the ending position.
        while((e = readdir(dir)) != NULL) {  //Iterates through the entire directory.
            struct stat info;                //Helps us know about stuff
            strcpy(EndPtr, e->d_name);       //Copies the current filedirOrigem to the end of the path, overwriting it with each loop.
            printf("%s \n", EndPtr);             //printing the root files

            //fileCopy(EndPtr, "EndPtr");
            if(!stat(Path, &info)) {         //stat returns zero on success.
                if(S_ISDIR(info.st_mode)) {  //Are we dealing with a directory?
                    //Make corresponding directory in the target folder here.
                    
                    treeCopy(Path,dirDestino);   //Calls this function AGAIN, this time with the sub-dirOrigem.
                } else if(S_ISREG(info.st_mode) ){ //Or did we find a regular file?
                    //fileCopy(, dirOrigem);
                    
                }
            }
        }


    closedir(dir);
    closedir(dirDest);
   
    
}

int main(void) {

    treeCopy("pasta","pasta-copia");
   

    return(0);
}