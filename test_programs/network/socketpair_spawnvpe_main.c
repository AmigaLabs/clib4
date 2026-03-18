/*
 * Esempio di comunicazione bidirezionale usando socketpair con spawnvpe
 * Questo è il programma PRINCIPALE che crea i socket e lancia il processo figlio
 */

#include <sys/types.h>
#include <sys/socket.h>
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
    int sockets[2];
    pid_t pid;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    // Crea una coppia di socket connessi per comunicazione bidirezionale
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == -1) {
        perror("socketpair");
        exit(EXIT_FAILURE);
    }

    printf("[MAIN] Socketpair creato: socket[0]=%d, socket[1]=%d\n", 
           sockets[0], sockets[1]);

    // Prepara gli argomenti per il processo figlio
    char *child_argv[] = {
        "./socketpair_child",
        NULL
    };

    printf("[MAIN] Lancio processo figlio con spawnvpe...\n");

    // Usa spawnvpe per lanciare il processo figlio
    // Passa il socket come stdin E stdout del figlio (socketpair è bidirezionale)
    // spawnvpe(file, argv, env, dir, stdin, stdout, stderr)
    pid = spawnvpe("./socketpair_child", (const char **)child_argv, NULL, NULL,
                   sockets[1],  // stdin del figlio
                   sockets[1],  // stdout del figlio (stesso socket, è bidirezionale!)
                   -1);         // stderr standard
    
    if (pid == -1) {
        perror("spawnvpe");
        close(sockets[0]);
        close(sockets[1]);
        exit(EXIT_FAILURE);
    }

    // Il processo padre chiude il socket che usa il figlio
    close(sockets[1]);

    printf("[MAIN] Processo figlio lanciato (PID: %d)\n", pid);

    // Invia messaggio al figlio
    const char *message = "Messaggio dal processo principale via socketpair";
    if (write(sockets[0], message, strlen(message)) == -1) {
        perror("[MAIN] write");
        close(sockets[0]);
        exit(EXIT_FAILURE);
    }
    printf("[MAIN] Messaggio inviato al figlio\n");

    // Riceve risposta dal figlio
    bytes_read = read(sockets[0], buffer, BUFFER_SIZE - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("[MAIN] Ricevuto dal figlio: '%s'\n", buffer);
    } else if (bytes_read == -1) {
        perror("[MAIN] read");
    }

    // Invia secondo messaggio
    const char *second_msg = "Secondo messaggio - conferma ricezione";
    if (write(sockets[0], second_msg, strlen(second_msg)) != -1) {
        printf("[MAIN] Secondo messaggio inviato\n");
    }

    // Riceve conferma finale
    bytes_read = read(sockets[0], buffer, BUFFER_SIZE - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("[MAIN] Conferma dal figlio: '%s'\n", buffer);
    }

    close(sockets[0]);

    // Aspetta la terminazione del processo figlio
    int status;
    waitpid(pid, &status, 0);
    
    if (WIFEXITED(status)) {
        printf("[MAIN] Processo figlio terminato con stato: %d\n", 
               WEXITSTATUS(status));
    }

    printf("[MAIN] Test socketpair con spawnvpe completato!\n");

    return EXIT_SUCCESS;
}
