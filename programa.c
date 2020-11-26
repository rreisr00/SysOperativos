#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>


int calculaAleatorios(int min, int max);
void dosisHandler(int sig);


int main(int argc, char* argv[]){
    pid_t pid, npid, pidwait, pidpaciente;
    int estado;
    pid_t *pacientes;
    srand(time(NULL));
    if(argc >= 2){
        pacientes = (pid_t *)malloc(sizeof(pid_t)*atoi(argv[1]));
    }

    pid = fork();
    if(pid == -1){
        perror("Llamada a fork()");
        exit(-1);
    }else if(pid == 0) /*MEDICO*/{
        int pacientesCurados = 0;
        //Crea a los procesos paciente
        for(int i = 0; i < argv[1]; i++) {
            pidpaciente = fork();
            if(pidpaciente == -1){
                perror("Llamada a fork()");
                exit(-1);
            }else if(pidpaciente == 0) /*PACIENTE*/{
                if(signal(SIGUSR1, dosisHandler) == SIG_ERR) {
                    perror("Llamada a signal");
                    exit(-1);
                }
                pause();
            }else{
                pacientes[i] = pidpaciente;
            }
        }// Aqui solo sale medico
        
        if(signal(SIGUSR2, SIG_DFL) == SIG_ERR){
            perror("Llamada a signal");
            exit(-1);
        }
        pause();
        for(int i = 0; i < argv[1]; i++){

        }
    }else /*EPIDEMIOLOGO*/{
        npid = fork();
        if(npid == -1){
            perror("Llamada a fork()");
            exit(-1);
        }else if(npidpid == 0)/*FARMACEUTICO*/{
            if(signal(SIGUSR1, dosisHandler); == SIG_ERR) {
                perror("La señal a fallado");
                exit(-1);
            }
            pause();
        }else /*EPIDEMIOLOGO*/{
            int dosis;
            sleep(2);

            estado = kill(npid, SIGUSR1); //manda la señal al farmaceutico
            if(kill == -1){
                perror("Llamada a kill()");
                exit(-1);
            }
            pidwait = wait(&estado);
            dosis = WEXITSTATUS(estado);
            if(dosis == 0){
                printf("No hay dosis suficiente para los pacientes");
                kill(SIGKILL,npid);
                kill(SIGKILL, pid);
                exit(0);
            }else{
                estado = kill(pid, SIGUSR2);
            }
        }

    }
}

void dosisHandler(int sig){
    int dosis = calculaAleatorios(0, 1);
    exit(dosis);
}
int calculaAleatorios(int min, int max){
    return rand() % (max-min+1) + min;
}

