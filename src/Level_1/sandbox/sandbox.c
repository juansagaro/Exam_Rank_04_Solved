/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sandbox.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsagaro- <jsagaro-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/07 12:52:48 by jsagaro-          #+#    #+#             */
/*   Updated: 2026/02/08 12:22:57 by jsagaro-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

/*
** Manejador de señal vacío.
** Su única función es existir para que SIGALRM interrumpa waitpid
** en lugar de matar al proceso padre.
*/
void handle_timeout(int sig)
{
    (void)sig;
}

int sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{
    pid_t   pid;
    int     status;
    struct sigaction sa;

    // 1. Configurar el manejador de señales para el Timeout (SIGALRM)
    // Queremos que cuando suene la alarma, el waitpid se interrumpa.
    sa.sa_handler = handle_timeout;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0; // Importante: NO usar SA_RESTART para que waitpid falle con EINTR
    
    if (sigaction(SIGALRM, &sa, NULL) == -1)
        return (-1);

    // 2. Crear el proceso hijo (El entorno seguro)
    pid = fork();
    if (pid == -1)
        return (-1);

    // --- PROCESO HIJO ---
    if (pid == 0)
    {
        // Ejecutamos la función sospechosa
        f();
        // Si termina bien, salimos con éxito (0)
        exit(0);
    }

    // --- PROCESO PADRE ---
    
    // 3. Programar la alarma
    alarm(timeout);

    // 4. Esperar al hijo
    // waitpid se quedará bloqueado hasta que el hijo termine O suene la alarma.
    if (waitpid(pid, &status, 0) == -1)
    {
        // Si waitpid falló por la alarma (Interrupción de llamada al sistema)
        if (errno == EINTR)
        {
            kill(pid, SIGKILL); // Matamos al hijo que sigue corriendo
            waitpid(pid, NULL, 0); // Limpiamos el zombie generado por kill
            
            if (verbose)
                printf("Bad function: timed out after %u seconds\n", timeout);
            return (0);
        }
        return (-1); // Otro error de waitpid
    }

    // 5. Desactivar la alarma (si el hijo terminó antes del tiempo)
    alarm(0);

    // 6. Analizar cómo murió el hijo
    if (WIFEXITED(status))
    {
        int exit_code = WEXITSTATUS(status);
        if (exit_code == 0)
        {
            if (verbose)
                printf("Nice function!\n");
            return (1);
        }
        else
        {
            if (verbose)
                printf("Bad function: exited with code %d\n", exit_code);
            return (0);
        }
    }
    else if (WIFSIGNALED(status))
    {
        if (verbose)
            printf("Bad function: %s\n", strsignal(WTERMSIG(status)));
        return (0);
    }

    return (0);
}
