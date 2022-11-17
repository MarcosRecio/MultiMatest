/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * user_text.c
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 */


/** @file user.c
 *
 * Este fichero contiene las funciones que se encargan de interactuar con el
 * usuario. Aquí se encuentran funciones para la entrada de datos (selección de
 * opciones principalmente) y para la salida de información: menús, mensajes,
 * etc.
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <libintl.h>
#define _(String) gettext (String)
#include "MaTest.h"


/*
 * Imprime una cabecera con el nombre del programa, versión y una descripción
 * breve.
 */
void menu_header (void)
{
	screen_clear();
	printf ("\n"
	        "                            --- MaTest %s ---\n"
	        "                        Tester de matrices lógicas\n"
	        "\n", VERSION);
}


/*
 * Procedimiento para definir de manera interactiva una conectiva unaria nueva
 * dada por su nombre en una lista de conectivas unarias. Se pregunta al usuario
 * por todos los valores uno por uno, indicando el subíndice del elemento
 * actual.
 * Si el usuario presiona Enter sin introducir ningún valor, se asigna
 * autómaticamente a la posición actual el último valor introducido.
 */
void
tui_ucon_add_custom (LlLogic* logic, const char *symbol)
{
	LlUCon    *ucon;
	char      *value;
	int       *matrix;
	int        i;

	matrix = (int*) calloc (DIM, sizeof (int));
	value = (char*) calloc (10, sizeof (char));

	/* Iniciamos todos los valores a -1 para el control de errores */
	for (i = 0; i < DIM; i++) {
		matrix[i] = -1;
	}

	/* Preguntamos por los nuevos valores, uno a uno */
	for (i = 0; i < DIM; i++) {
		do {
			printf (" %s %i: ", symbol, i);
			value = fgets (value, 9, stdin);
			/* si se pulsa Enter sin introducir un valor, si no es el primero, se toma
			 * el último introducido */
			if (value && value[0] == '\n') {
				if (i == 0) {
					matrix[i] = -1;
				}
				else {
					matrix[i] = matrix[i - 1];
					printf (" %s %i: %i\n", symbol, i, matrix[i]);
				}
			}
			else {
				sscanf (value, "%i", &matrix[i]);
			}
		}
		while (matrix[i] < 0 || matrix[i] >= DIM);
	}

	ucon = ll_ucon_new (symbol, symbol, matrix, DIM);
	logic->ucons = ll_ucon_list_append (logic->ucons, ucon);
	free (matrix);
	free (value);
}


/*
 * Procedimiento para definir de manera interactiva una conectiva binaria nueva
 * dada por su nombre en una lista de conectivas binarias. Se pregunta al
 * usuario por todos los valores uno por uno, indicando los subíndices del
 * elemento actual.
 * Si el usuario presiona Enter sin introducir ningún valor, se asigna
 * autómaticamente a la posición actual el último valor introducido.
 */
void
tui_bcon_add_custom (LlLogic* logic, const char *symbol)
{
	LlBCon    *bcon;
	char      *value;
	int       **matrix;
	int        i, j;

	value = (char*) calloc (10, sizeof (char));
	matrix = (int**) calloc (DIM, sizeof (int*));
	for (i = 0; i < DIM; i++) {
		matrix[i] = calloc (DIM, sizeof (int));
	}

	/* Iniciamos todos los valores a -1 para el control de errores */
	for (i = 0; i < DIM; i++) {
		for (j = 0; j < DIM; j++) {
			matrix[i][j] = -1;
		}
	}

	/* Preguntamos por los nuevos valores, uno a uno */
	for (i = 0; i < DIM; i++) {
		for (j = 0; j < DIM; j++) {
			do {
				printf (" %s %i %i: ", symbol, i, j);
				value = fgets (value, 9, stdin);
				/* si se pulsa Enter sin introducir un valor, si no es el primer valor,
				 * se toma el último introducido */
				if (value && value[0] == '\n') {
					if (j == 0 && i == 0) {
						matrix[i][j] = -1;
					}
					else if (j == 0) {
						matrix[i][j] = matrix[i - 1][MAXV];
						printf (" %s %i %i: %i\n", symbol, i, j, matrix[i][j]);
					}
					else {
						matrix[i][j] = matrix[i][j - 1];
						printf (" %s %i %i: %i\n", symbol, i, j, matrix[i][j]);
					}
				}
				else {
					sscanf (value, "%i", &matrix[i][j]);
				}
			}
			while (matrix[i][j] < 0 || matrix[i][j] >= DIM);
		}
	}

	bcon = ll_bcon_new (symbol, symbol, matrix, DIM);
	logic->bcons = ll_bcon_list_append (logic->bcons, bcon);
	free (value);
	free (matrix);
}


/*
 * Imprime todas las matrices de todas las conectivas, primero las unarias,
 * luego las binarias, en el orden en que se hayen definidas.
 */
void
print_matrices (LlLogic* logic)
{
	GList    *iter;
	gchar    *straux;

	for (iter = logic->ucons; iter; iter = iter->next) {
		straux = sprint_ucon_matrix ((LlUCon*) iter->data, logic);
		printf ("%s", straux);
		g_free (straux);
		printf ("\n");
	}

	for (iter = logic->bcons; iter; iter = iter->next) {
		straux = sprint_bcon_matrix ((LlBCon*) iter->data, logic);
		printf ("%s", straux);
		g_free (straux);
		printf ("\n");
	}
}


/*
 * Función que imprime la información del trabajo actual, con los siguientes
 * elementos:
 *   - tamaño de las matrices
 *   - mínimo valor designado
 *   - conectivas unarias definidas, separadas por espacios
 *   - conectivas binarias definidas, idem
 *   - fórmula que se evaluará
 *   - tipo de evaluación
 *
 * @param work La función recibe como argumento un registro de tipo Work.
 */
void menu_info (Work* work)
{
	GList    *iter;

	printf (_("  Dimensión de las matrices:  "));
	if (work->DIM) {
		printf ("%ix%i", work->DIM, work->DIM);
	}
	else {
		printf (_("Sin definir"));
	}

	printf (_("\n  Mínimo Valor Designado:     "));
	if (work->MDV) {
		printf ("%i", work->MDV);
	}
	else {
		printf (_("Sin definir"));
	}

	printf (_("\n  Conectivas unarias:         "));
	for (iter = work->logic->ucons; iter; iter = iter->next) {
		printf ("%s ", ((LlUCon*) iter->data)->symbol);
	}

	printf (_("\n  Conectivas binarias:        "));
	for (iter = work->logic->bcons; iter; iter = iter->next) {
		printf ("%s ", ((LlBCon*) iter->data)->symbol);
	}

	if (work->formula[0]) {
		printf (_("\n  Fórmula:                    %s"), work->formula);
	}
	else {
		printf (_("\n  Fórmula:                    Sin definir"));
	}

	switch (work->evaluation_style) {
		case ALL :
			printf (_("\n  Evaluar valores:            [Todos]  Designados  No designados"));
			break;
		case DESIGNATED :
			printf (_("\n  Evaluar valores:            Todos  [Designados]  No designados"));
			break;
		case NOT_DESIGNATED :
			printf (_("\n  Evaluar valores:            Todos  Designados  [No designados]"));
			break;
	}

	printf ("\n\n");
}


/*
 * Procedimiento que imprime el menú de opciones.
 */
void menu_options (void)
{
	printf (_(" Opciones:\n"
	          "  D: Redefinir Dimensión de las matrices\n"
						"  M: Redefinir el Mínimo Valor Designado\n"
						"  V: Valores evaluados\n"
						"\n"
						"  N: Nueva/Editar conectiva         P: Imprimir las matrices por pantalla\n"
						"  K: Borrar conectiva               W: Escribir las matrices en un archivo\n"
						"\n"
						"  F: Introducir Fórmula\n"
						"  E: Evaluar\n"
						"                                                    A: Acerca de    Q: Salir\n"
						"\n"
						" Select your option: "));
}


/*
 * Procedimiento para requerir la dimensión inicial de las matrices, para
 * definir las conectivas. Aparece al inicio del programa.
 */
void menu_dimension (void)
{
	printf (_(" Defina primero la dimensión de las matrices para empezar a trabajar.\n"
						" Después, el programa definirá automáticamente las conectivas más comunes\n"
						" según el modelo de las lógicas multivaluadas de Łukasiewicz.\n\n"));
}


/*
 * Imprime el texto de la página "Acerca de", ésta contiene una descripción del
 * programa, versión, créditos y nota legal.
 */
void menu_about (void)
{
	screen_clear();
	printf (_("\n"
	          " MaTest versión %s\n"
	          "\n"
	          " AUTOR: César González Gutiérrez <ceguel@gmail.com>\n"
	          "\n"
	          " DESCRIPCIÓN: MaTest (Matrix Tester) es un tester de matrices lógicas, libre\n"
	          "  y multiplataforma. Definidas unas conectivas como matrices, un mínimo valor\n"
	          "  designado y dada una fórmula bien formada, calcula todas las valoraciones de\n"
	          "  dicha fórmula y evalúa en cada caso si se trata de un valor designado o no.\n"
	          "  Sucesor de Matrigüity, creado por J.M. Méndez y B. García Noriega en 1982.\n"
	          "\n"
	          " COPYRIGHT (C) 2008-2010 - César González Gutiérrez <ceguel@gmail.com>.\n"
	          "  MaTest es software libre: usted puede redistribuirlo y/o modificarlo\n"
            "  bajo los términos de la Licencia Pública General GNU publicada\n"
            "  por la Fundación para el Software Libre, ya sea la versión 3\n"
            "  de la Licencia, o (a su elección) cualquier versión posterior.\n"
            "\n"
            "  MaTest se distribuye con la esperanza de que sea útil, pero\n"
            "  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita\n"
            "  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO.\n"
            "  Consulte los detalles de la Licencia Pública General GNU para obtener\n"
            "  una información más detallada.\n"
            "\n"), VERSION);
}


/*
 * Imprime el menú global: cabecera, información del trabajo y opciones.
 */
void menu_index (Work* work)
{
	menu_header();
	menu_info (work);
	menu_options();
}


/*
 * Modo interactivo en modo texto.
 */
int
mode_tui (Work* work)
{
	char     opt,
	         symbol[4],
	        *answer,
	         formula[BUFSIZ],
	         namefile[BUFSIZ],
          *buffer = NULL;
	int      newdim;
	FILE    *outfile;

	answer = (char*) calloc (BUFSIZ, sizeof (char));

	/* Manejador de opciones interactivo */
	for (;;) {
		menu_index (work);
		opt = readin (answer, "dmvpwnkfeahq");

		switch (opt) {
			/* Redefinir la dimensión de las matrices */
			case 'd':
				do {
					printf (_(" Nueva dimensión de las matrices ( <1 ): "));
					answer = fgets (answer, BUFSIZ, stdin);
					if (answer) {
						sscanf (answer, "%i", &newdim);
					}
				}
				while (answer && answer[0] != '\n' && newdim < 2);
				if (answer[0] == '\n') {
					break;
				}
				ll_ucon_list_free (work->logic->ucons);
				work->logic->ucons = NULL;
				ll_bcon_list_free (work->logic->bcons, work->DIM);
				work->logic->bcons = NULL;
				work->DIM = newdim;
				work->MDV = work->MAXV;
				ll_logic_set_default_ucons_lukasiewicz (work->logic);
		    ll_logic_set_default_bcons_lukasiewicz (work->logic);
				/*
				if (work->logic_modified)
					{
						printf (_(" Se han modificado las matrices por defecto.\n"
						          " ¿Desea guardar los cambios? (s/N)\n"));
						opt = readin (answer, "ysn\n");
						switch (opt)
							{
								case 'y':
								case 's':
									write_matrices (file, work->logic);
								case 'n':
					*/
		    break;

			/* Cambiar los valores mostrados en la evaluación */
			case 'v':
				screen_clear ();
				menu_header ();
				menu_info (work);
				printf (_(" ¿Qué valores quiere que sean evaluados?\n"
				          " a: Todos, d: Designados, n: No designados: "));
				opt = readin (answer, "adn\n");
				switch (opt) {
					case 'a':
						work->evaluation_style = ALL;
						break;
					case 'd':
						work->evaluation_style = DESIGNATED;
						break;
					case 'n':
						work->evaluation_style = NOT_DESIGNATED;
						break;
					default:
						break;
				}
				break;

			/* Cambiar el Mínimo Valor Designado */
			case 'm':
				screen_clear ();
				menu_header ();
				menu_info (work);
				do {
					printf (_(" Nuevo Mínimo Valor Designado [1 - %i]: "), work->MAXV);
					answer = fgets (answer, BUFSIZ, stdin);
					if (answer) {
						sscanf (answer, "%i", &work->MDV);
					}
				}
				while (work->MDV < 1 || work->MDV > work->MAXV);
				break;

			/* Imprimir las matrices por pantalla */
			case 'p':
				screen_clear ();
				print_matrices (work->logic);
				make_pause ();
				break;

			/* Escribir las matrices en un archivo externo */
			case 'w':
				screen_clear ();
				menu_header ();
				menu_info (work);
				printf (_(" Nombre del archivo donde escribir: "));
				answer = fgets (answer, BUFSIZ, stdin);
				if (answer) {
					sscanf (answer, "%s", namefile);
				}
				if (answer[0] == '\n') {
					break;
				}
				/* Comprueba que el archivo existe */
				outfile = fopen (namefile, "r");
				if (outfile) {
					printf (_(" El archivo \'%s\' ya existe, ¿desea sobreescribirlo? (s/N): "), namefile);
					opt = readin (answer, "sy\n");
					if (opt == 'y' || opt == 's') {
						outfile = freopen (namefile, "w", outfile);
						if (outfile) {
							write_matrices (outfile, work->logic);
							fclose (outfile);
						}
						else {
							printf (_("\n No tiene permisos para escribir en este archivo.\n\n"));
							make_pause ();
						}
					}
					else {
						fclose (outfile);
						fprintf (stderr, _("\n Error: no fue posible abrir el archivo.\n\n"));
					}
				}
				else {
					outfile = fopen (namefile, "w");
					if (outfile) {
						write_matrices (outfile, work->logic);
						fclose (outfile);
					}
					else {
						perror (_("\n Error: no se pudo escribir en el archivo.\n\n"));
						make_pause ();
					}
				}
				break;

			/* Definir una conectiva nueva personalizada o editar una existente */
			case 'n':
				screen_clear ();
				menu_header ();
				menu_info (work);

				printf (_(" Símbolo para la conectiva: "));
				opt = toupper (readin (answer, "abcdefghijklmnopqrstuvwxyz\n"));
				sprintf (symbol, "%c", opt);
				if (opt == '\n') {
					break;
				}
				else if (ll_ucon_list_get_ucon_by_symbol (work->logic->ucons, symbol)) {
					printf (_(" Ya hay una conectiva unaria con el mismo símbolo, ¿desea redefinirla? (s/N): "));
					opt = readin (answer, "ys\n");
					if (opt == 's' || opt == 'y') {
						ll_con_delete_by_symbol (work->logic, symbol);
						tui_ucon_add_custom (work->logic, symbol);
						work->logic_modified = true;
					}
				}
				else if (ll_bcon_list_get_bcon_by_symbol (work->logic->bcons, symbol)) {
					printf (_(" Ya hay una conectiva binaria con el mismo símbolo, ¿desea redefinirla? (s/N): "));
					opt = readin (answer, "ys\n");
					if (opt == 's' || opt == 'y') {
						ll_con_delete_by_symbol (work->logic, symbol);
						tui_bcon_add_custom (work->logic, symbol);
						work->logic_modified = true;
					}
				}
				else {
					printf (_(" ¿Conectiva unaria o binaria? (u/b): "));
					opt = readin (answer, "ub\n");
					switch (opt) {
						case 'u':
							tui_ucon_add_custom (work->logic, symbol);
							work->logic_modified = true;
							break;
						case 'b':
							tui_bcon_add_custom (work->logic, symbol);
							work->logic_modified = true;
							break;
					}
				}
				break;

			/* Eliminar una conectiva existente */
			case 'k':
				screen_clear ();
				menu_header ();
				menu_info (work);

				printf (_(" Símbolo de la conectiva a borrar: "));
				opt = toupper (readin (answer, "abcdefghijklmnopqrstuvwxyz\n"));
				ll_con_delete_by_symbol (work->logic, &opt);
				break;


			/* Pedir una fórmula bien formada y pasársela al parser */
			case 'f':
				screen_clear();
				menu_header();
				menu_info (work);
				do {
					printf (_("\n Escriba una fórmula: "));
					fgets (answer, BUFSIZ, stdin);
					sscanf (answer, "%s", formula);
				}
				while (formula[0] && !ll_formula_is_wff_pn (formula, work->logic));

				if (!formula[0])
					break;
				else
					strcpy (work->formula, formula);

				//ll_logic_add_formula_vars (work->logic, work->formula_pn);

				//if (work->wff) {
					//ll_wff_free (work->wff);
					//work->wff = NULL;
				//}
				ll_wff_parse_formula (&work->wff, work->formula, work->logic);

				formula[0] = '\0';
				break;

			/* Hacer la evaluación */
			case 'e':
				if (!work->wff) {
					printf (_("\n No se ha introducido ninguna fórmula bien formada.\n\n"));
				}
				else {
					screen_clear ();
					buffer = (gchar*) sprint_evaluation (work);
					printf ("%s\n", buffer);
					g_free (buffer);
				}
				make_pause ();
				break;

			/* Mostrar la página "Acerca de" */
			case 'a':
				menu_about ();
				make_pause ();
				break;

			/* Salir pidiendo confirmación */
			case 'q':
				if (work->logic_modified) {
					printf (_(" Se han hecho cambios en la lógica actual. ¿Salir sin guardar? (s/N): "));
					fgets (answer, BUFSIZ, stdin);
					if (answer[0] == 's' || answer[0] == 'y') {
						return 0;
					}
					else {
						break;
					}
				}
				else {
					return 0;
				}
		}
	}
	
	free (answer);
}
