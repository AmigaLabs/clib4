/*
 * Esempio di comunicazione bidirezionale usando pipe con spawnvpe
 * Dimostra l'uso di pipe per IPC (Inter-Process Communication) bidirezionale
 * NOTA: Per comunicazione bidirezionale servono 2 pipe (una per direzione)
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#define BUFFER_SIZE 256

int main(int argc, char *argv[])
{
    int pipe_parent_to_child[2];  // Pipe: padre -> figlio
    int pipe_child_to_parent[2];  // Pipe: figlio -> padre
    pid_t pid;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    // Crea prima pipe: padre scrive, figlio legge
    if (pipe(pipe_parent_to_child) == -1) {
        perror("pipe (parent to child)");
        exit(EXIT_FAILURE);
    }

    // Crea seconda pipe: figlio scrive, padre legge
    if (pipe(pipe_child_to_parent) == -1) {
        perror("pipe (child to parent)");
        close(pipe_parent_to_child[0]);
        close(pipe_parent_to_child[1]);
        exit(EXIT_FAILURE);
    }

    printf("[MAIN] Pipe create:\n");
    printf("       Padre->Figlio: read=%d, write=%d\n", 
           pipe_parent_to_child[0], pipe_parent_to_child[1]);
    printf("       Figlio->Padre: read=%d, write=%d\n", 
           pipe_child_to_parent[0], pipe_child_to_parent[1]);

    // Prepara argomenti per il processo figlio
    char *child_argv[] = {
        "./pipe_child",
        NULL
    };

    printf("[MAIN] Lancio processo figlio con spawnvpe...\n");

    // Lancia il processo figlio passando le pipe come stdin/stdout
    // spawnvpe(file, argv, env, dir, stdin, stdout, stderr)
    // Il figlio riceverà: stdin=pipe_parent_to_child[0], stdout=pipe_child_to_parent[1]
    pid = spawnvpe("./pipe_child", (const char **)child_argv, NULL, NULL,
                   pipe_parent_to_child[0],  // stdin del figlio
                   pipe_child_to_parent[1],  // stdout del figlio
                   -1);                       // stderr standard
    
    if (pid == -1) {
        perror("spawnvpe");
        close(pipe_parent_to_child[0]);
        close(pipe_parent_to_child[1]);
        close(pipe_child_to_parent[0]);
        close(pipe_child_to_parent[1]);
        exit(EXIT_FAILURE);
    }

    // Il padre chiude i file descriptor non utilizzati
    close(pipe_parent_to_child[0]);  // Non legge da questa pipe
    close(pipe_child_to_parent[1]);  // Non scrive su questa pipe

    printf("[MAIN] Processo figlio lanciato (PID: %d)\n", pid);

    // Invia messaggio al figlio
    const char *message = "Messaggio dal padre via pipe";
    if (write(pipe_parent_to_child[1], message, strlen(message)) == -1) {
        perror("[MAIN] write");
    } else {
        printf("[MAIN] Messaggio inviato al figlio\n");
    }

    // Riceve risposta dal figlio
    bytes_read = read(pipe_child_to_parent[0], buffer, BUFFER_SIZE - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("[MAIN] Ricevuto dal figlio: '%s'\n", buffer);
    } else if (bytes_read == -1) {
        perror("[MAIN] read");
    }

    // Invia secondo messaggio
    const char *second_msg = "Secondo messaggio - conferma";
    if (write(pipe_parent_to_child[1], second_msg, strlen(second_msg)) != -1) {
        printf("[MAIN] Secondo messaggio inviato\n");
    }

    // Riceve conferma finale
    bytes_read = read(pipe_child_to_parent[0], buffer, BUFFER_SIZE - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("[MAIN] Conferma dal figlio: '%s'\n", buffer);
    }

    // Chiude le pipe
    close(pipe_parent_to_child[1]);
    close(pipe_child_to_parent[0]);

    // Aspetta la terminazione del processo figlio
    int status;
    waitpid(pid, &status, 0);
    
    if (WIFEXITED(status)) {
        printf("[MAIN] Processo figlio terminato con stato: %d\n", 
               WEXITSTATUS(status));
    }

    printf("[MAIN] Test pipe bidirezionale con spawnvpe completato!\n");

    return EXIT_SUCCESS;
}
