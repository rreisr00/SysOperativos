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

    //Se crea un proceso hijo de
    pid = fork();
    if(pid == -1){
        perror("Llamada a fork()");
        exit(-1);
    }else if(pid == 0) /*MEDICO*/{
        if(signal(SIGUSR2, SIG_DFL) == SIG_ERR){
            perror("Llamada a signal");
            exit(-1);
        }
        signal(SIGKILL, SIG_DFL);
        //Espera a que el proceso epidemiologo mande la señal
        pause();
        //Crea a los procesos paciente
        int darDosis, waitpaciente, reaccion, npacientesreaccion = 0;
        for(int i = 0; i < argv[1]; i++) {
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
        }// Aqui solo sale medico

        for(int i = 0; i < npacientes; i++){
            printf("Medico vacuna al paciente %d\n", i+1);
            darDosis = kill(pacientes[i], SIGUSR1);
            if(darDosis == -1){
                perror("Llamada a kill()\n");
                exit(-1);
            }
            waitpaciente = wait(&estado);
            if(waitpaciente = -1){
                perror("Llamada a wait()\n");
                exit(-1);
            }
            reaccion = WEXITSTATUS(estado);
            if(reaccion%2 = 0){
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
            if(signal(SIGUSR1, dosisHandler); == SIG_ERR) {
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
            if(dosis == 0){
                printf("No hay dosis suficiente para los pacientes");
                kill(npid, SIGKILL);
                kill(pid, SIGKILL);
                exit(0);
            }else{
                estado = kill(pid, SIGUSR2);
                pidwait = wait(&estado);
                printf("Se han curado %d paciente(s)\n", WEXITSTATUS(estado));
            }
        }

    }
}
void reaccionHandler(int sig){
    sleep(2);
    int reaccion = calculaAleatorios(1, 10);
    exit(reaccion);
}
void dosisHandler(int sig){
    int dosis = calculaAleatorios(0, 1);
    exit(dosis);
}
int calculaAleatorios(int min, int max){
    return rand() % (max-min+1) + min;
}

