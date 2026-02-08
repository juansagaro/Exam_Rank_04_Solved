/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   argo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsagaro- <jsagaro-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/07 15:00:24 by jsagaro-          #+#    #+#             */
/*   Updated: 2026/02/08 11:52:21 by jsagaro-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "given.h"

// ==========================================
// Pega esto debajo de las funciones proporcionadas
// (debajo de expect y antes de serialize/main)
// ==========================================

// Declaración adelantada para la recursividad
int parse_value(json *dst, FILE *stream);

// Función auxiliar para leer una cadena raw (devuelve char*)
// Se usa tanto para valores de tipo STRING como para las CLAVES de los mapas.
char *get_string_content(FILE *stream)
{
    char *buffer = NULL;
    size_t length = 0;
    size_t capacity = 0;

    if (!accept(stream, '"'))
        return NULL;

    while (1)
    {
        int c = getc(stream);
        if (c == EOF)
        {
            unexpected(stream);
            free(buffer);
            return NULL;
        }
        
        if (c == '"')
            break; // Fin de la cadena

        if (c == '\\')
        {
            c = getc(stream); // Leer el carácter escapado
            if (c == EOF)
            {
                unexpected(stream);
                free(buffer);
                return NULL;
            }
        }

        // Gestión de memoria del buffer
        if (length + 1 >= capacity)
        {
            capacity = (capacity == 0) ? 16 : capacity * 2;
            char *new_buf = realloc(buffer, capacity);
            if (!new_buf)
            {
                free(buffer);
                return NULL;
            }
            buffer = new_buf;
        }
        buffer[length++] = (char)c;
    }

    // Terminar con null
    if (length + 1 >= capacity)
    {
        char *new_buf = realloc(buffer, length + 1);
        if (!new_buf) { free(buffer); return NULL; }
        buffer = new_buf;
    } else if (buffer == NULL) {
        // Caso cadena vacía ""
        buffer = calloc(1, 1);
    }
    
    buffer[length] = '\0';
    return buffer;
}

int parse_string(json *dst, FILE *stream)
{
    char *s = get_string_content(stream);
    if (!s)
        return -1;
    
    dst->type = STRING;
    dst->string = s;
    return 1;
}

int parse_int(json *dst, FILE *stream)
{
    int val;
    // fscanf devuelve el número de items leídos correctamente
    if (fscanf(stream, "%d", &val) != 1)
    {
        unexpected(stream);
        return -1;
    }
    dst->type = INTEGER;
    dst->integer = val;
    return 1;
}

int parse_map(json *dst, FILE *stream)
{
    if (!accept(stream, '{'))
        return -1;

    dst->type = MAP;
    dst->map.data = NULL;
    dst->map.size = 0;

    // Caso de mapa vacío {}
    if (accept(stream, '}'))
        return 1;

    while (1)
    {
        // 1. Leer Clave (debe ser string)
        if (peek(stream) != '"')
        {
            unexpected(stream);
            return -1;
        }

        char *key = get_string_content(stream);
        if (!key)
            return -1;

        // 2. Esperar ':'
        if (!expect(stream, ':'))
        {
            free(key);
            return -1;
        }

        // 3. Preparar espacio en el array para el nuevo par
        struct pair *new_data = realloc(dst->map.data, (dst->map.size + 1) * sizeof(pair));
        if (!new_data)
        {
            free(key);
            return -1;
        }
        dst->map.data = new_data;

        // Asignar la clave al nuevo hueco
        dst->map.data[dst->map.size].key = key;

        // 4. Leer Valor recursivamente
        // Pasamos la dirección de la parte 'value' del nuevo par
        if (parse_value(&dst->map.data[dst->map.size].value, stream) != 1)
        {
            // Nota: La limpieza general se hará en el main si devolvemos -1,
            // pero la clave actual debe liberarse para evitar leaks intermedios si el main no limpia profundo.
            // Dado el free_json proporcionado, simplemente devolvemos error y dejamos que el llamador limpie.
            return -1;
        }
        
        dst->map.size++;

        // 5. Verificar si hay más elementos o cierre
        if (accept(stream, ','))
            continue; // Hay más elementos
        
        if (accept(stream, '}'))
            break; // Fin del mapa

        unexpected(stream);
        return -1;
    }
    return 1;
}

// Función dispatcher principal (y recursiva)
int parse_value(json *dst, FILE *stream)
{
    int c = peek(stream);
    
    if (c == '{')
        return parse_map(dst, stream);
    
    if (c == '"')
        return parse_string(dst, stream);
    
    if (isdigit(c) || c == '-')
        return parse_int(dst, stream);
    
    unexpected(stream);
    return -1;
}

// Función de entrada pedida por el enunciado
int argo(json *dst, FILE *stream)
{
    // 1. Parsear el JSON
    if (parse_value(dst, stream) != 1)
        return -1;
    
    // 2. Verificar que no haya basura al final (EOF)
    // El enunciado dice: "unexpected token" o "unexpected end of input" 
    // si hay cosas raras. Aquí verificamos que hayamos terminado limpiamente.
    if (peek(stream) != EOF)
    {
        unexpected(stream);
        return -1;
    }
    
    return 1;
}

void	serialize(json j)
{
	switch (j.type)
	{
		case INTEGER:
			printf("%d", j.integer);
			break ;
		case STRING:
			putchar('"');
			for (int i = 0; j.string[i]; i++)
			{
				if (j.string[i] == '\\' || j.string[i] == '"')
					putchar('\\');
				putchar(j.string[i]);
			}
			putchar('"');
			break ;
		case MAP:
			putchar('{');
			for (size_t i = 0; i < j.map.size; i++)
			{
				if (i != 0)
					putchar(',');
				serialize((json){.type = STRING, .string = j.map.data[i].key});
				putchar(':');
				serialize(j.map.data[i].value);
			}
			putchar('}');
			break ;
	}
}

int	main(int argc, char **argv)
{
	if (argc != 2)
		return 1;
	char *filename = argv[1];
	FILE *stream = fopen(filename, "r");
	json	file;
	if (argo (&file, stream) != 1)
	{
		free_json(file);
		return 1;
	}
	serialize(file);
	printf("\n");
}
