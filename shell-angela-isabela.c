/*****************************************************************
Universidade Federal de São Carlos - Campus Sorocaba
Laboratórios de Sistemas Operacionais - Turma B

Projeto 02 - Gerencia de Projetos

Autores:  Ângela Rodrigues Ferreira  RA: 552070
	  Isabela Salmeron Boschi	 RA: 552593
*****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX 256
#define SIM 1
#define NAO 0

int main() {

	char entrada[MAX];
	char *aux;
	char *args[MAX];
	int i=0, qtdArgs=0;
	int salvar = NAO, segplano = NAO, lerArq = NAO;
	int indiceler, indicesalvar;
	int pid;


  	while (1) {
		//Inicializa as variaveis
		i=0; qtdArgs=0;	
		salvar = NAO; segplano = NAO; lerArq = NAO;
		
	    printf("> ");
	    scanf("%[^\n]s", entrada); while(getchar() != '\n');
			
	    if (!strcmp(entrada, "exit")) {
	      exit(EXIT_SUCCESS);
	    }

	    //Pega o primeiro token
		aux = strtok(entrada," ");

		//Args[0] contem o nome do arquivo
		//Args[...] contem os argumentos
		while(aux != NULL){	
			args[i]=aux;
			aux = strtok(NULL, " ");			
			i++;
			qtdArgs++;
		}
		args[qtdArgs] = NULL; //Mostra onde termina os argumentos.
	  

	    //Procura pelos caracteres especiais :)
	  	for(i=0; i < qtdArgs; i++){
	  		//Verifica se tem execucao em segundo plano
	  		if(strcmp(args[i], "&") == 0){
	  			segplano = SIM;
	  			//Cria um processo
	  			pid = fork();
		  		if(pid){
		  			//Nao espera o filho
		  			waitpid(pid, NULL, WNOHANG);
		  		}else{
		  			//executa o processo em segundo plano
					execlp(args[0], args[0], NULL);
		      		printf("Erro ao executar comando!\n");
		      		exit(EXIT_FAILURE);
		  		}
	  			break;
	  		} 
	  		//Verifica se vai ler a partir de um arquivo
	  		else if(strcmp(args[i], "<") == 0){
	  			lerArq = SIM;
	  			indiceler = i;
	  			//break;
	  		}
	  		//Verifica se vai escrever em arquivo
	  		else if(strcmp(args[i], ">") == 0){
	  			salvar = SIM;
	  			indicesalvar = i;
	  			//break;
	  		}
	  	}
	  	
	  	//Se nao tiver arquivos em segundo plano ele executa o comando
	  	if(segplano == NAO) {
	  		//Cria um processo
		  	pid = fork();
	  		if (pid){
	  			waitpid(pid, NULL, 0); //Espera pelo filho
	  		}else{
	  			//Se houver < e > habilita a leitura a partir d um arquivo e escrita em arquivo
	  			if(lerArq == SIM && salvar == SIM){
					//Abre o arquivo para leitura da entrada
			  		freopen(args[indiceler+1], "r", stdin);
			  		args[indiceler] = NULL;
			  		//Abre o arquivo para salvar a saida
			  		freopen(args[indicesalvar+1], "w", stdout);
			  		args[indicesalvar] = NULL;

	  			}
	  			//Se houver apenas leitura a partir de entrada
		  		else if( lerArq == SIM){
			  		//Abre o arquivo para leitura da entrada
			  		freopen(args[indiceler+1], "r", stdin);
			  		args[indiceler] = NULL;
		  		}
		  		//Se for salvar a saida em algum arquivo
		  		else if(salvar == SIM){
			  		//Abre o arquivo para salvar a saida
			  		freopen(args[indicesalvar+1], "w", stdout);
			  		args[indicesalvar] = NULL;
			  		
			  	}
			  	
	  			execvp(args[0], args); //Executa os comandos
	      		printf("Erro ao executar comando!\n");
	      		exit(EXIT_FAILURE);
	  		}	
	  	}
    }
}
