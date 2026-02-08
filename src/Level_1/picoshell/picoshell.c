/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   picoshell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsagaro- <jsagaro-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/07 12:51:32 by jsagaro-          #+#    #+#             */
/*   Updated: 2026/02/07 12:52:36 by jsagaro-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int picoshell(char **cmds[])
{
    int i = 0;
    int fd[2];
    int tmp_fd;
    pid_t pid;

    // Guardamos el original para restaurarlo después
    tmp_fd = dup(STDIN_FILENO);

    while (cmds[i])
    {
        // 1. Crear pipe si no es el último
        if (cmds[i + 1])
        {
            if (pipe(fd) == -1)
            {
                // ERROR HANDLING: Restaurar y limpiar antes de salir
                dup2(tmp_fd, STDIN_FILENO);
                close(tmp_fd);
                return (1);
            }
        }

        // 2. Fork
        pid = fork();
        if (pid == -1)
        {
            // ERROR HANDLING: Restaurar y limpiar antes de salir
            dup2(tmp_fd, STDIN_FILENO);
            close(tmp_fd);
            return (1);
        }

        if (pid == 0) // HIJO
        {
            // El hijo hereda el STDIN modificado del padre automáticamente.
            // Solo nos preocupamos de la salida (STDOUT).
            if (cmds[i + 1])
            {
                close(fd[0]); 
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);
            }
            
            // Importante cerrar el backup del stdin en el hijo también
            // (es una copia que heredó y no necesita)
            close(tmp_fd); 

            execvp(cmds[i][0], cmds[i]);
            exit(1);
        }
        else // PADRE
        {
            if (cmds[i + 1])
            {
                close(fd[1]); // Cerrar escritura
                // AQUÍ ESTÁ LA MAGIA:
                // Cambiamos el STDIN del padre. El próximo fork heredará esto.
                // dup2 cierra automáticamente el STDIN anterior (la tubería vieja).
                dup2(fd[0], STDIN_FILENO); 
                close(fd[0]); // Cerrar lectura (ya está en STDIN)
            }
            i++;
        }
    }

    // Esperar a todos
    while (wait(NULL) != -1);

    // RESTAURACIÓN FINAL
    dup2(tmp_fd, STDIN_FILENO);
    close(tmp_fd);

    return (0);
}

// int main()
// {
// 	char *cmd1[] = {"ls", "-l", NULL};
// 	char *cmd2[] = {"sort", NULL};
// 	char *cmd3[] = {"wc", "-l", NULL};
// 	char **cmds[] = {cmd1, cmd2, cmd3, NULL};

// 	return picoshell(cmds);
// }
