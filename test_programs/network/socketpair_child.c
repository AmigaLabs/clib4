/*
 * Esempio di comunicazione bidirezionale usando socketpair con spawnpe
 * Questo è il programma FIGLIO lanciato dal processo principale
 */

#include <sys/types.h>
#include <sys/socket.h>
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
    printf("[CHILD] Uso stdin/stdout (fd=%d/%d) per comunicare tramite socketpair\n",
           STDIN_FILENO, STDOUT_FILENO);

    // Riceve messaggio dal processo padre (da stdin)
    bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("[CHILD] Ricevuto dal padre: '%s'\n", buffer);
    } else if (bytes_read == -1) {
        perror("[CHILD] read");
        exit(EXIT_FAILURE);
    } else {
        printf("[CHILD] Connessione chiusa dal padre\n");
        exit(EXIT_SUCCESS);
    }

    // Invia risposta al padre (su stdout, che è lo stesso socket bidirezionale!)
    const char *response = "Risposta dal processo figlio (lanciato con spawnvpe)";
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
    const char *confirm = "OK - Terminazione processo figlio";
    if (write(STDOUT_FILENO, confirm, strlen(confirm)) != -1) {
        printf("[CHILD] Conferma finale inviata\n");
    }

    printf("[CHILD] Terminazione\n");

    return EXIT_SUCCESS;
}
