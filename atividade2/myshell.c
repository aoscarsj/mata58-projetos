#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

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

// funcao para separar a linha em comandos
char **divideLinha(char *linha){
	char **tokens = (char **)malloc(sizeof(char *) * MAX_CHARS+1);
	char *token;

    // \t: tab
    // \r: carriage return
    // \n: line feed (new line)
    // \a: system bell

	char delim[10] = " \t\n\r\a";
	int posicaoArray = 0, tamanhoBuffer = MAX_CHARS+1;
	if (!tokens){
		printf("\nErro na alocação do buffer.");
		exit(EXIT_FAILURE);
	}
	token = strtok(linha, delim);
	while (token != NULL){
		tokens[posicaoArray] = token;
		posicaoArray ++;
		if (posicaoArray >= tamanhoBuffer){
			tamanhoBuffer += MAX_CHARS+1;
			linha = realloc(linha, tamanhoBuffer * sizeof(char *));
			if (!linha){ // Erro na alocação do buffer.
			
			printf("\nErro na alocação do buffer.");
			exit(EXIT_FAILURE);
			}
		}
		token = strtok(NULL, delim);
	}
	tokens[posicaoArray] = NULL;
	return tokens;
}

// funcao para criar um processo filho e rodar o comando
int myShellIniciar (char **args){
	pid_t pid, wpid;
	int status;
	pid = fork();
	if (pid == 0){
		// processo filho
		//As  funções  execv e execvp fornecem um vetor de ponteiros para strings terminadas em '\0'
       		//que representam a lista de argumentos disponível ao novo programa.  O primeiro  argumento,
       		//por  convenção,  deveria  apontar  para  o  nome  do  arquivo  associado  ao arquivo sendo
       		//executado.  O vetor de ponteiros deve ser terminado por um ponteiro NULL .
		if (execvp(args[0], args) == -1){
			printf("myShell: comando desconhecido: %s\n", (*args));
		}
	exit(EXIT_FAILURE);
	}
	else if (pid < 0){
		//forkando o erro
		perror("myShell: ");
	}
	else{
		// processo pai
	do {
	/*A função wait suspende a execução do processo até a morte de seu filho. Se o filho já estiver morto no instante da chamada da primitiva (caso de um processo zumbi, abortado mais a frente), a função retorna imediatamente.

	A função waitpid suspende a execução do processo até que o filho especificado pelo argumento pid tenha morrido. Se ele já estiver morto no momento da chamada, o comportamento é idêntico ao descrito anteriormente.

	O valor do argumento pid pode ser:

	< -1 : significando que o pai espera a morte de qualquer filho cujo o ID do grupo é igual so valor de pid;
	-1 : significando que o pai espera a morte de qualquer filho;
	0 : significando que o pai espera a morte de qualquer processo filho cujo ID do grupo é igual ao do processo chamado;
	> 0 : significando que o pai espera a morte de um processo filho com um valor de ID exatamente igual a pid.
	Se status é não nulo (NULL), wait e waitpid armazena a informação relativa a razão da morte do processo filho, sendo apontada pelo ponteiro status. Este valor pode ser avaliado com diversas macros que são listadas com o comando shell man 2 wait.*/
      wpid = waitpid(pid, &status, WUNTRACED);
    }
     while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	return 1;
}

// implementacao dos comandos built in
int myShell_cd(char **args){

	if (args[1] != NULL) {
		if (chdir(args[1]) != 0) {
			perror("myShell");
		}
	}
	else {
		printf("myShell: o comando cd necessita de argumento \n");
	} 
	
	return 1;
}


// lidando com os comandos built in
char *builtin_cmd[] = {"cd"};// comandos

int (*builtin_func[]) (char **) = {&myShell_cd}; // array de funcoes com os ponteiros para chamar a partir do exexShell

int qtdBuiltin(){ // funcao para retornar o numero de comandos built in 

	return sizeof(builtin_cmd)/sizeof(char *);
}

// funcao para executar o comando no terminal
int execShell(char **args){
	int ret;
	if (args[0] == NULL){
		// Comando vazio
		return 1;
	}
	// loop para checar por funcoes built in
	for (int i=0; i< qtdBuiltin(); i++){ // qtdBuiltin() retorna o numero de funcoes built in
	
		if(strcmp(args[0], builtin_cmd[i])==0) // checando se a funcao do usuario combina com o nome da funcao
			return (*builtin_func[i])(args); // chama a respectiva funcao com argumentos
	}
	ret = myShellIniciar(args);
	return ret;
}

int main(int argc, char const *argv[]) {
    //  TODO:rotina de inicialização
    char raw_line[MAX_CHARS + 1]; // 4096 chars + null
    char ** args;
    while(true) {
        printf("myshell> "), fflush(stdout); // imprime e força a saída imediata na tela
        if(fgets(raw_line, sizeof raw_line ,stdin) && raw_line[0] != '\n'){


        	if(strcmp("exit\n",raw_line) == 0){ //a string raw_line ta retornando com o \n no final
        		return EXIT_SUCCESS;
        	}

        	args=divideLinha(raw_line);
        	execShell(args);
        	free(args);

        	
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
            // erroEntrada(); // imprime mensagem de erro correspondente
        }
    }

    if (errno != 0)
        return -1;

    return 0;
    // se encerrou com quit, exit ou EOF return 0;
    // se aconteceu algum erro, retornar -1
}
