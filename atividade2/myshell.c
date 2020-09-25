#include <stdio.h>

/* FUNÇÕES A SE IMPLEMENTAR
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

    /* TODO:  LOOP PRINCIPAL
        printf("myshell> "), fflush(stdout); // imprime e força a saída imediata na tela
        // ler entrada (pode ser vazio (apenas apertar enter), comandos, ou EOF) e extrair seu conteudo (separar palavras) com strtok
        // se for EOF, "exit" ou "quit" encerrar o loop e encerrar retornando 0
        // se for em branco: continue;
        ////
            se for comando, executar o comando com os argumentos passados
            o shell precisa monitorar os comandos que executou para reportar
            o status de saída, e também com os comandos wait, waitfor e watchdog
            ele acompanha a execução de um processo ate o fim
        ////
        // se foi algo incorreto (comando inexistente, etc) imprimir mensagem de erro

    */

    // se encerrou com quit, exit ou EOF return 0;
    // se aconteceu algum erro, retornar -1
}
