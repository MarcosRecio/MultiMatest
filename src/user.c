/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * user.c
 * This file is part of MaTest
 *
 * Copyright (C) 2008-2011 - César González Gutiérrez <ceguel@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 */


#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <libintl.h>
#define _(String) gettext (String)
#include "MaTest.h"


/*
 * Busca el primer caracter que coincida con un patrón dado por una cadena de
 * caracteres. Pensado para recoger una opción dada por el usuario por teclado.
 * Esta función hace la entrada insensible a mayúsculas, devuelve siempre
 * minúsculas.
 *
 * @param str Cadena en la que buscar.
 * @param pattern Patrón dado como cadena de caracteres.
 * @return Caracter encontrado; si es una letra, en minúscula.
 */
char
readin (char *str, const char *pattern)
{
	char ch;
	int i;

	fflush (stdout);
	for (;;)
	{
		fgets (str, BUFSIZ, stdin);
		ch = '\0';
		for (i = 0; str[i]; i++)
		{
			if (isupper (ch = str[i]))
				ch = tolower (ch);
			if (strchr (pattern, ch))
				return ch;
		}
	}
}


/*
 * Procedimiento para limpiar la pantalla. Pretende ser multiplataforma,
 * utiliza según qué método dependiendo del parámetro que se le pase al
 * preprocesador. Para sistemas tipo Unix se llama a la función 'clear',
 * para Windows/DOS se llama a la función 'cls', por defecto se imprime una
 * serie de 24 saltos de línea.
 */
void
screen_clear (void)
{
#ifdef UNIX
	if (system ("clear"))
		return;
#elif WIN32
	if (system ("cls"))
		return;
#else
	int i;
	for (i = 0; i < 24; i++)
		printf ("\n");
#endif
}


/*
 * Procedimiento para hacer una pausa en la salida por la terminal.
 * Imprime un mensaje y espera hasta que el usuario pulse Intro.
 */
void
make_pause (void)
{
	printf (_("Pulse Intro para continuar... "));
	fflush (stdout);
	while (getchar() != '\n');
}


/*
 * Imprime la versión del programa. Esta función es llamada desde la línea de
 * comandos, según lo aconsejado por la FSF.
 */
void
menu_version (void)
{
	printf (_("MaTest %s\n"
						"Copyright (C) 2008, 2009 - César González Gutiérrez.\n"
						"License GPLv3+: GNU General Public Licence version 3 or later,\n"
						"see <http://gnu.org/licenses/gpl.html>.\n"
						"This is free software: you are free to change and redistribute it.\n"
						"There is NO WARRANTY, to the extent permitted by law.\n"), VERSION);
}


/*
 * Imprime el uso del programa
 */
void
menu_usage (void)
{
	printf (_("Uso:\n"
	          "      matest [opciones] [archivo]\n"
	          "\n"
	          "Opciones:\n"
	          "      -h, --help                            muestra esta ayuda y termina\n"
	          "      -v, --version                         muestra la versión y termina\n"
	          "      -g, --gui                             con interfaz gráfica (por defecto)\n"
	          "      -t, --text                            modo interactivo en modo texto\n"
	          "      -d, --dimmension   <entero>           dimensión de las matrices\n"
	          "      -m, --mdv          <entero>           mínimo valor designado\n"
	          "      -f, --formula      <formula>          fórmula que se evaluará\n"
	          "      -e, --evaluate                        valores que se evaluarán:\n"
	          "                         a, all               - todos\n"
	          "                         d, designated        - designados\n"
	          "                         n, not-designated    - no designados\n"));
}


/*
 * Procedimiento que imprime la nota legal que debe aparecer al inicio del modo
 * interactivo, como aconseja la FSF.
 */
void
menu_init (void)
{
	menu_header();
	printf (_("\n"
						" Copyright (C) 2008, 2009 - César González Gutiérrez.\n"
						" License GPLv3+: GNU General Public Licence version 3 or later,\n"
						" see <http://gnu.org/licenses/gpl.html>.\n"
						" This is free software: you are free to change and redistribute it.\n"
						" There is NO WARRANTY, to the extent permitted by law.\n"
						" For more details type 'a' in the main menu.\n\n\n"));
}


/*
 * Lector de archivos con matrices.
 */
// FIXME Comentar el código
// FIXME Liberar la memoria de las conectivas usadas si algo falla
int
read_matricesfile (Work *work, char *filename)
{
	FILE       *file;
	GList      *ucon_list = NULL,
	           *bcon_list = NULL;
	LlUCon     *ucon;
	LlBCon     *bcon;
	char        con_symbol[SLEN + 1],
	            strsymb[SLEN + 1],
	            auxsymb[BUFSIZ/2],
	            line[BUFSIZ], *auxstr,
	            value[BUFSIZ/2];
	int         i, j, d, dim = 0, mdv = 0,
	            row = 0, col = 0,
	            vector[BUFSIZ/2],
	           **mtx = NULL;

	/* Comprobamos que el fichero se pueda abrir para lectura. */
	file = fopen (filename, "r");
	if (file == NULL) {
		perror ("El archivo no existe o no tiene permisos para su lectura.\n");
		return 1;
	}

	while (!feof (file)) {
		while (fgets (line, BUFSIZ, file)) {
			if (sscanf (line, "%s", auxsymb) != EOF) {
				if (auxsymb[0] == '#') {
					printf ("Se ha encontrado un comentario. Pasamos a la siguiente línea.\n");
					break;
				}
				else if (strcmp ("MDV", auxsymb) == 0) {
					sscanf (line, "%s%u", auxsymb, &mdv);
				}
				else if (!isdigit (auxsymb[0])) {
					if (ll_ucon_list_get_ucon_by_symbol (ucon_list, con_symbol) ||
					    ll_bcon_list_get_bcon_by_symbol (bcon_list, con_symbol)) {
						fprintf (stderr, "Definición reiterada de la conectiva %s.\n", con_symbol);
						return 6;
					}
					else {
						printf ("No hay un símbolo de conectiva declarado. Se toma \'%s\'.\n", auxsymb);
						if (strlen (auxsymb) > SLEN) {
							printf ("El símbolo de la conectiva encontrado es demasiado largo.\n");
							return 10;
						}
						else {
							strcpy (con_symbol, auxsymb);
						}
					}
					break;
				}
				else if (isdigit (auxsymb[0])) {
					col = 0;
					auxstr = line;
					while (sscanf (auxstr, "%u", &vector[col]) != EOF) {
						printf ("Se ha encontrado el valor \"%i\".\n", vector[col]);
						sprintf (value, "%u", vector[col]);
						auxstr = strstr (auxstr, value);
						auxstr = auxstr + strlen (value);
						col++;
					}
					for (i = 0; i < col; i++) {
						printf ("v[%i]:%i ", i, vector[i]);
					}
					printf ("Guardado.\n");
					if (col < 2) {
						printf ("Muy pocos elementos para una conectiva unaria.\n");
						return 11;
					}
					else {
						d = col;
					}
					printf ("Comprobamos dimension.\n");
					if (dim == 0) {
						dim = d;
						printf ("Definida dimension = %i.\n", dim);
					}
					else if (dim != d) {
						perror ("El número de elementos de una fila es distinto al esperado.\n");
						return 3;
					}
					printf ("Declaramos la matriz si no existía.\n");
					if (!mtx) {
						mtx = (int**) calloc (dim, sizeof (int*));
						for (i = 0; i < dim; i++) {
							mtx[i] = (int*) calloc (dim, sizeof (int));
						}
						mtx[1][0] = -1;
						printf ("Declarada.\n");
					}
					else {
						printf ("Ya estaba declarada.\n");
						if (row < d)
							row++;
						else {
							printf ("Demasiadas filas para una matriz.\n");
							return 8;
						}
					}
					if (row > dim) {
						perror ("Una matriz tiene más filas de las esperadas.\n");
						return 4;
					}
					for (i = 0; i < dim; i++) {
						if (vector[i] > dim) {
							perror ("Hay elementos con valor mayor que la dimensión de su matriz.\n");
							return 5;
						}
					}
					for (col = 0; col < dim; col++)
						mtx[row][col] = vector[col];
					for (i = 0; i < dim; i++)
						printf ("[%i][%i]:%i ", row, i, mtx[row][i]);
					putchar ('\n');
				}
			}
			else if (sscanf (line, "%s", auxsymb) == EOF) {
				if (mtx) {
						if (mtx[1][0] == -1 && con_symbol[0] != '\0') {
							printf ("* Guardamos una conectiva unaria.\n");
							ucon = ll_ucon_new (con_symbol, con_symbol, mtx[0], dim);
							ucon_list = ll_ucon_list_append (ucon_list, ucon);
							con_symbol[0] = '\0';
							row = 0;
							for (i = 0; i < dim; i++) {
								free (mtx[i]);
							}
							free (mtx);
							mtx = NULL;
						}
						else if (con_symbol[0] != '\0')	{
							printf ("** Guardamos una conectiva binaria.\n");
							bcon = ll_bcon_new (con_symbol, con_symbol, mtx, dim);
							bcon_list = ll_bcon_list_append (bcon_list, bcon);
							con_symbol[0] = '\0';
							row = 0;
							for (i = 0; i < dim; i++) {
								free (mtx[i]);
							}
							free (mtx);
							mtx = NULL;
						}
						else {
							printf ("Hay una matriz sin símbolo asociado.\n");
							return 9;
						}
					}
			}
		}
	}
	if (mtx) {
		if (mtx[1][0] == -1 && con_symbol[0] != '\0') {
			printf ("* Guardamos una conectiva unaria.\n");
			ucon = ll_ucon_new (con_symbol, con_symbol, mtx[0], dim);
			ucon_list = ll_ucon_list_append (ucon_list, ucon);
			con_symbol[0] = '\0';
			row = 0;
			for (i = 0; i < dim; i++) {
				free (mtx[i]);
			}
			free (mtx);
			mtx = NULL;
		}
		else if (con_symbol[0] != '\0')	{
			printf ("** Guardamos una conectiva binaria.\n");
			bcon = ll_bcon_new (con_symbol, con_symbol, mtx, dim);
			bcon_list = ll_bcon_list_append (bcon_list, bcon);
			con_symbol[0] = '\0';
			row = 0;
			for (i = 0; i < dim; i++) {
				free (mtx[i]);
			}
			free (mtx);
			mtx = NULL;
		}
		else {
			printf ("Hay una matriz sin símbolo asociado.\n");
			return 9;
		}
	}
	
	ll_ucon_list_free (work->logic->ucons);
	work->logic->ucons = ucon_list;
	ll_bcon_list_free (work->logic->bcons, work->DIM);
	work->logic->bcons = bcon_list;
	work->DIM = dim;
	if (mdv > 0 && mdv < dim) {
		work->MDV = mdv;
	}
	else {
		work->MDV = work->MAXV;
	}
	work->logic_modified = TRUE;
	return 0;
}
