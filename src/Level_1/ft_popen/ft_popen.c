/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_popen.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsagaro- <jsagaro-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/07 12:52:50 by jsagaro-          #+#    #+#             */
/*   Updated: 2026/02/07 18:21:05 by jsagaro-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int ft_popen(const char *file, const char *argv[], char type)
{
    int pipefd[2];
    pid_t pid;

    // 1. Validación de parámetros
    if (!file || !argv || (type != 'r' && type != 'w'))
        return (-1);

    // 2. Crear la tubería (pipe)
    // pipefd[0] es LECTURA, pipefd[1] es ESCRITURA
    if (pipe(pipefd) == -1)
        return (-1);

    // 3. Crear el proceso hijo (Fork)
    pid = fork();
    if (pid == -1)
    {
        close(pipefd[0]);
        close(pipefd[1]);
        return (-1);
    }

    // --- PROCESO HIJO ---
    if (pid == 0)
    {
        if (type == 'r')
        {
            // El padre quiere LEER, así que el hijo debe ESCRIBIR.
            // Conectamos el extremo de escritura del pipe al STDOUT del hijo.
            dup2(pipefd[1], STDOUT_FILENO);
            
            // Cerramos los extremos originales del pipe (limpieza)
            close(pipefd[0]);
            close(pipefd[1]);
        }
        else if (type == 'w')
        {
            // El padre quiere ESCRIBIR, así que el hijo debe LEER.
            // Conectamos el extremo de lectura del pipe al STDIN del hijo.
            dup2(pipefd[0], STDIN_FILENO);
            
            // Cerramos los extremos originales
            close(pipefd[0]);
            close(pipefd[1]);
        }
        
        // Reemplazamos el proceso con el comando nuevo
        execvp(file, (char * const *)argv);
        
        // Si execvp falla, debemos salir para no tener dos procesos 'padre'
        exit(1);
    }

    // --- PROCESO PADRE ---
    else
    {
        if (type == 'r')
        {
            // Queremos leer, cerramos el extremo de escritura que no usamos
            close(pipefd[1]);
            // Devolvemos el extremo de lectura
            return (pipefd[0]);
        }
        else // type == 'w'
        {
            // Queremos escribir, cerramos el extremo de lectura que no usamos
            close(pipefd[0]);
            // Devolvemos el extremo de escritura
            return (pipefd[1]);
        }
    }
}

// int main()
// {
//     int  fd;
//     char *line;

//     fd = ft_popen("ls", (const char **){"ls", NULL}, 'r');
//     while ((line = get_next_line(fd)))
//         ft_putstr(line);
//     return (0);
// }
