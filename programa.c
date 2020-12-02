#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>


int calculaAleatorios(int min, int max);
void reaccionHandler(int sig);
void dosisHandler(int sig);
void medicoHandler(int sig);

int main(int argc, char* argv[]){
	pid_t pid, npid, pidwait, pidpaciente;
	int estado, npacientes;
	pid_t *pacientes;
	srand(time(NULL));

	//Se crea el puntero a pacientes que guardara el pid de los pacientes creados por medico
	if(argc >= 2){
		npacientes = atoi(argv[1]);
		pacientes = (pid_t *)malloc(sizeof(pid_t)*npacientes);
	}
	printf("Numero de pacientes: %d\n", npacientes);

	//Se crea un proceso hijo del proceso principal epidemiologo
	pid = fork();
	if(pid == -1){
		//Si Falla la llamada a fork()
		perror("Llamada a fork()");
		exit(-1);
	}else if(pid == 0) /*MEDICO*/{
		if(signal(SIGUSR2, medicoHandler) == SIG_ERR){
			perror("Llamada a signal");
			exit(-1);
		}

		//Espera a que el proceso epidemiologo mande la señal
		pause();
		printf("Creamos a los procesos pacientes\n");

		//Crea a los procesos paciente
		int darDosis, waitpaciente, reaccion, npacientesreaccion = 0, estadoPaciente;
		for(int i = 0; i < npacientes; i++) {
			pidpaciente = fork();
			if(pidpaciente == -1){
				perror("Llamada a fork()");
				exit(-1);
			}else if(pidpaciente == 0) /*PACIENTE*/{
				if(signal(SIGUSR1, reaccionHandler) == SIG_ERR) {
					perror("Llamada a signal");
					exit(-1);
				}
				
				pause();
			}else{
				pacientes[i] = pidpaciente;
			}
		}
		for(int i = 0; i < npacientes; i++){
			printf("Paciente PID: %d\n", pacientes[i]);
		}
		
		for(int i = 0; i < npacientes; i++){
			sleep(1);
			printf("Medico vacuna al paciente %d\n", i+1);
			
			darDosis = kill(pacientes[i], SIGUSR1);
			do{
				waitpaciente = wait(&estadoPaciente);
			}while(waitpaciente == -1);
			reaccion = WEXITSTATUS(estadoPaciente);
			printf("estado: %d\n", estadoPaciente);
			printf("reaccion: %d\n", reaccion);
			if(reaccion%2 == 0){
				printf("Paciente ha tenido reaccion\n");
				npacientesreaccion += 1;
			}
		}
		exit(npacientesreaccion);
	}else /*EPIDEMIOLOGO*/{
		npid = fork();
		if(npid == -1){
			perror("Llamada a fork()");
			exit(-1);
		}else if(npid == 0)/*FARMACEUTICO*/{
			if(signal(SIGUSR1, dosisHandler) == SIG_ERR) {
				perror("La señal a fallado");
				exit(-1);
			}
			pause();
		}else /*EPIDEMIOLOGO*/{
			int dosis, killestado;
			sleep(2);
			killestado = kill(npid, SIGUSR1); //manda la señal al farmaceutico
			if(killestado == -1){
				perror("Llamada a kill()");
				exit(-1);
			}
			pidwait = wait(&estado);
			dosis = WEXITSTATUS(estado);
			printf("Dosis : %d\n",dosis);
			if(dosis == 0){
				printf("No hay dosis suficiente para los pacientes\n");
				kill(npid, SIGKILL);
				kill(pid, SIGKILL);
				exit(0);
			}else{
				estado = kill(pid, SIGUSR2);
				pidwait = wait(&estado);
				if(pidwait == -1){
					perror("Llamada a wait");
					exit(-1);
				}
				printf("Se han curado %d paciente(s)\n", WEXITSTATUS(estado));
			}
		}
	}
}


void reaccionHandler(int sig){
	srand(time(NULL)^getpid());
	sleep(2);
	int reaccion = calculaAleatorios(1, 10);
	printf("reaccionHandler: %d\n", reaccion);
	printf("Aleatorio: %d\n", calculaAleatorios(1, 10));
	exit(reaccion);
}

void dosisHandler(int sig){
	int dosis = calculaAleatorios(0, 1);
	exit(dosis);
}

int calculaAleatorios(int min, int max){
	return rand() % (max-min+1) + min;
}

void medicoHandler(int sig){
	return;
}
