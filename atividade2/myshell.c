#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#define MAX_CHARS 4096
/* FUNÇÕES A SE IMPLEMENTAR
erroEntrada() // imprime mensagem de erro relacionada à entrada
start()
wait()
waitfor()
run()
watchdog()
chdir()
pwd()
*/

int main(int argc, char const *argv[]) {
    //  TODO:rotina de inicialização
    char raw_line[MAX_CHARS + 1]; // 4096 chars + null
    char * args[100];
    while(true) {
        printf("myshell> "), fflush(stdout); // imprime e força a saída imediata na tela
        if(fgets(raw_line, sizeof raw_line ,stdin) && raw_line[0] != '\n'){
            /*
            ////
            se for comando, executar o comando com os argumentos passados
            o shell precisa monitorar os comandos que executou para reportar
            o status de saída, e também com os comandos wait, waitfor e watchdog
            ele acompanha a execução de um processo ate o fim
            ////
            // se foi algo incorreto (comando inexistente, etc) imprimir mensagem de erro
            // se for "exit" ou "quit" encerrar o loop e encerrar retornando 0
            */
        }else if (feof(stdin)){
            errno == 0;
            break;
        }else if (raw_line[0] == '\n'){
            continue;
            // se for em branco: continue;
        }else{
            erroEntrada(); // imprime mensagem de erro correspondente
        }
    }

    if (errno != 0)
        return -1;

    return 0;
    // se encerrou com quit, exit ou EOF return 0;
    // se aconteceu algum erro, retornar -1
}
