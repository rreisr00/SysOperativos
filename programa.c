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

	printf("Comenzando...\n");
	//Se crea el puntero a pacientes que guardara el pid de los pacientes creados por medico
	if(argc >= 2){
		npacientes = atoi(argv[1]);
		pacientes = (pid_t *)malloc(sizeof(pid_t)*npacientes);
	}
	printf("Numero de pacientes: %d\n", npacientes);

	//Se crea un proceso hijo del proceso principal epidemiologo
	pid = fork();
	if(pid == -1){
		perror("Llamada a fork()");
		exit(-1);
	}else if(pid == 0) /*MEDICO*/{
		printf("El Medico(%d) esta esperando...\n", getpid());
		if(signal(SIGUSR2, medicoHandler) == SIG_ERR){
			perror("Llamada a signal");
			exit(-1);
		}

		//Espera a que el proceso epidemiologo mande la señal
		pause();

		//Crea a los procesos paciente
		int darDosis, waitpaciente, reaccion, npacientesreaccion = 0, estadoPaciente;
		printf("\n");
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
				//El proceso paciente espera hasta que le llegue la señal de medico
				printf("Paciente(%d) esperando...\n", getpid());
				pause();
			}else{
				sleep(0.5);
				pacientes[i] = pidpaciente;
			}
		}
		printf("\n");
		for(int i = 0; i < npacientes; i++){
			printf("Medico(%d) vacuna al Paciente %d(%d)\n", getpid(), i+1, pacientes[i]);
			
			darDosis = kill(pacientes[i], SIGUSR1);

			//Se espera hasta que uno de los procesos hijos acabe
			
			do{
				waitpaciente = wait(&estadoPaciente);
			}while(waitpaciente == -1);

			reaccion = WEXITSTATUS(estadoPaciente);
			if(reaccion%2 == 0){
				printf("Paciente(%d) ha tenido reaccion\n", pacientes[i]);
				npacientesreaccion += 1;
			}else{
				printf("Paciente(%d) no ha tenido reaccion\n", pacientes[i]);
			}
		}
		printf("\nMedico(%d) terminando...\n", getpid());
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
			printf("El Farmaceutico(%d) esta esperando...\n", getpid());
			pause();
		}else /*EPIDEMIOLOGO*/{
			int dosis, killestado;
			printf("Epidemiologo(%d) pregunta a farmaceutico si hay dosis\n", getpid());
			sleep(2);
			killestado = kill(npid, SIGUSR1); //manda la señal al farmaceutico
			pidwait = wait(&estado);
			dosis = WEXITSTATUS(estado);
			if(dosis == 0){
				printf("No hay dosis suficiente para los pacientes\n");
				//Se mata a los procesos hijos
				kill(pid, SIGKILL);
				exit(0);
			}else{
				printf("Hay suficientes dosis\n");
				printf("Epidemiologo(%d) pide que medico empiece las vacunas\n", getpid());
				int senalMedico = kill(pid, SIGUSR2);// manda la señal a medico
				pidwait = wait(&estado);
				if(pidwait == -1){
					perror("Llamada a wait");
					exit(-1);
				}
				printf("Se han curado %d paciente(s) de %d pacientes\n", WEXITSTATUS(estado), npacientes);
			}
		}
	}
	printf("Terminando\n");
	return 0;
}


void reaccionHandler(int sig){
	//Genera un semilla que depende del proceso
	srand(time(NULL)^getpid());

	printf("Paciente(%d) ha sido vacunado, esperando reacción...\n", getpid());
	sleep(2);
	int reaccion = calculaAleatorios(1, 10);
	exit(reaccion);
}

void dosisHandler(int sig){
	int dosis = calculaAleatorios(0, 1);
	printf("El Farmaceutico(%d) comprueba si hay suficientes dosis\n", getpid());
	exit(dosis);
}

int calculaAleatorios(int min, int max){
	return rand() % (max-min+1) + min;
}

void medicoHandler(int sig){
	return;
}
