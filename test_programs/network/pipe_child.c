/*
 * Processo figlio per l'esempio di comunicazione bidirezionale con pipe
 * Lanciato dal processo principale tramite spawnpe
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define BUFFER_SIZE 256

int main(int argc, char *argv[])
{
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    printf("[CHILD] Processo figlio avviato (PID: %d)\n", getpid());
    printf("[CHILD] Uso stdin (fd=%d) e stdout (fd=%d) per comunicare\n", 
           STDIN_FILENO, STDOUT_FILENO);

    // Riceve primo messaggio dal padre (da stdin)
    bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("[CHILD] Ricevuto dal padre: '%s'\n", buffer);
    } else if (bytes_read == -1) {
        perror("[CHILD] read");
        exit(EXIT_FAILURE);
    } else {
        printf("[CHILD] Pipe chiusa dal padre\n");
        exit(EXIT_SUCCESS);
    }

    // Invia risposta al padre (su stdout)
    const char *response = "Risposta dal figlio tramite pipe";
    if (write(STDOUT_FILENO, response, strlen(response)) == -1) {
        perror("[CHILD] write");
        exit(EXIT_FAILURE);
    }
    printf("[CHILD] Risposta inviata al padre\n");

    // Riceve secondo messaggio
    bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("[CHILD] Secondo messaggio ricevuto: '%s'\n", buffer);
    }

    // Invia conferma finale
    const char *confirm = "OK - Figlio termina comunicazione";
    if (write(STDOUT_FILENO, confirm, strlen(confirm)) != -1) {
        printf("[CHILD] Conferma finale inviata\n");
    }

    printf("[CHILD] Terminazione\n");

    return EXIT_SUCCESS;
}
