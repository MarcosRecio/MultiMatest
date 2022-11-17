/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * connectives.c
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

/** @file connectives.c
 *
 * Este archivo contiene el código responsable de manejar las conectivas, su
 * creación, definición, eliminación...
 * Muchas de las funciones aquí presentes vienen por pares, unas para manejar
 * las conectivas unarias y otras para las conectivas binarias, unas manejan
 * vectores, las otras matrices.
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "logics.h"


/**
 * Crea una conectiva unaria nueva.
 *
 * @param symbol Símbolo asociado a la conectiva.
 * @param matrix Matriz de la conectiva.
 * @param dimension Dimensión de contexto para la matriz.
 */
LlUCon*
ll_ucon_new (const char *symbol, const char *symbol_pn, int *matrix, int dimension)
{
	LlUCon  *ucon;
	int     i;

	ucon = g_slice_new (LlUCon);
	ucon->matrix = g_new (int, dimension);

	strcpy (ucon->symbol, symbol);
	strcpy (ucon->symbol_pn, symbol_pn);
	for (i = 0; i < dimension; i++) {
		ucon->matrix[i] = matrix[i];
	}

	return ucon;
}


/**
 * Crea una conectiva binaria nueva.
 *
 * @param symbol Símbolo asociado a la conectiva.
 * @param matrix Matriz de la conectiva.
 * @param dimension Dimensión de contexto para la matriz.
 */
LlBCon*
ll_bcon_new (const char *symbol, const char *symbol_pn, int **matrix, int dimension)
{
	LlBCon    *bcon;
	int       i, j;

	bcon = g_slice_new (LlBCon);
	bcon->matrix = g_new (int*, dimension);
	for (i = 0; i < dimension; i++) {
		bcon->matrix[i] = g_new (int, dimension);
	}

	strcpy (bcon->symbol, symbol);
	strcpy (bcon->symbol_pn, symbol_pn);
	for (i = 0; i < dimension; i++) {
		for (j = 0; j < dimension; j++) {
			bcon->matrix[i][j] = matrix[i][j];
		}
	}

	return bcon;
}


/**
 * @brief Libera la memoria de una conectiva unaria.
 * @param ucon Una conectiva unaria.
 * 
 * Libera la memoria de una conectiva unaria dada como argumento.
 */
void
ll_ucon_free (LlUCon* ucon)
{
	g_free (ucon->matrix);
	g_slice_free (LlUCon, ucon);
}


/**
 * @brief Libera la memoria de una conectiva binaria.
 * @param bcon Una conectiva binaria.
 * @param dimension Dimensión de la matriz de la conectiva.
 */
void
ll_bcon_free (LlBCon* bcon, guint dimension)
{
	guint    i;

	for (i = 0; i < dimension; i++) {
		g_free (bcon->matrix[i]);
	}
	g_free (bcon->matrix);
	g_slice_free (LlBCon, bcon);
}


/**
 * Busca una conectiva unaria dada por su símbolo asociado dentro de una lista
 * de conectivas unarias.
 *
 * @param ucon_list Lista de conectivas unarias donde buscar.
 * @param symbol Símbolo por el que buscar.
 * @return Devuelve un puntero a la conectiva, si existe, o el puntero nulo en
 *         caso contrario.
 */
LlUCon*
ll_ucon_list_get_ucon_by_symbol (GList* ucon_list, const char *symbol)
{
	GList     *iter = NULL;
	LlUCon    *ucon;

	for (iter = ucon_list; iter; iter = iter->next) {
		ucon = (LlUCon*) iter->data;
		if (!g_ascii_strcasecmp (symbol, ucon->symbol)) {
			return ucon;
		}
	}

	return NULL;
}


/**
 * Busca una conectiva unaria dada por su símbolo asociado dentro de una lista
 * de conectivas unarias.
 *
 * @param ucon_list Lista de conectivas unarias donde buscar.
 * @param symbol Símbolo por el que buscar.
 * @return Devuelve un puntero a la conectiva, si existe, o el puntero nulo en
 *         caso contrario.
 */
LlUCon*
ll_ucon_list_get_ucon_by_symbol_pn (GList* ucon_list, const char *symbol_pn)
{
	GList     *iter = NULL;
	LlUCon    *ucon;

	for (iter = ucon_list; iter; iter = iter->next) {
		ucon = (LlUCon*) iter->data;
		if (!g_ascii_strcasecmp (symbol_pn, ucon->symbol_pn)) {
			return ucon;
		}
	}

	return NULL;
}


/**
 * Busca una conectiva binaria dada por su símbolo asociado dentro de una lista
 * de conectivas binarias.
 *
 * @param bcon_list Lista de conectivas binarias donde buscar.
 * @param symbol Símbolo por el que buscar.
 * @return Devuelve un puntero a la conectiva, si existe, o el puntero nulo en
 *         caso contrario.
 */
LlBCon*
ll_bcon_list_get_bcon_by_symbol (GList* bcon_list, const char *symbol)
{
	GList     *iter = NULL;
	LlBCon    *bcon;

	for (iter = bcon_list; iter; iter = iter->next) {
		bcon = (LlBCon*) iter->data;
		if (!g_ascii_strcasecmp (symbol, bcon->symbol)) {
			return bcon;
		}
	}

	return NULL;
}


/**
 * Busca una conectiva binaria dada por su símbolo asociado dentro de una lista
 * de conectivas binarias.
 *
 * @param bcon_list Lista de conectivas binarias donde buscar.
 * @param symbol Símbolo por el que buscar.
 * @return Devuelve un puntero a la conectiva, si existe, o el puntero nulo en
 *         caso contrario.
 */
LlBCon*
ll_bcon_list_get_bcon_by_symbol_pn (GList* bcon_list, char* symbol)
{
	GList     *iter = NULL;
	LlBCon    *bcon;

	for (iter = bcon_list; iter; iter = iter->next) {
		bcon = (LlBCon*) iter->data;
		if (!g_ascii_strcasecmp (symbol, bcon->symbol_pn)) {
			return bcon;
		}
	}

	return NULL;
}


/**
 * Borra una lista de conectivas unarias.
 *
 * @param ucon_list Lista de conectivas unarias a borrar.
 */
void
ll_ucon_list_free (GList* ucon_list)
{
	g_list_foreach (ucon_list, (GFunc) ll_ucon_free, NULL);
	g_list_free (ucon_list);
}


/**
 * Borra una lista de conectivas binarias.
 *
 * @param bcon_list Lista de conectivas binarias a borrar.
 */
void
ll_bcon_list_free (GList* bcon_list, int dimension)
{
	g_list_foreach (bcon_list, (GFunc) ll_bcon_free, GINT_TO_POINTER (dimension));
	g_list_free (bcon_list);
}


/**
 * Añade una conectiva unaria al final de una lista de conectivas unarias
 * siempre y cuando no exista ya una con el mismo símbolo.
 *
 * @param ucon_list Una lista de conectivas unarias.
 * @param ucon Una conectiva unaria.
 */
GList*
ll_ucon_list_append (GList* ucon_list, LlUCon* ucon)
{
	/* Descartamos la conectiva si hay otra con el mismo nombre */
	if (ll_ucon_list_get_ucon_by_symbol (ucon_list, ucon->symbol)) {
		return ucon_list;
	}
	/* Añadimos la conectiva al final de la lista */
	else {
		return g_list_append (ucon_list, ucon);
	}
}


/**
 * Añade una conectiva binaria al final de una lista de conectivas binarias
 * siempre y cuando no exista ya una con el mismo símbolo.
 */
GList*
ll_bcon_list_append (GList* bcon_list, LlBCon* bcon)
{
	/* Descartamos la conectiva si hay otra con el mismo nombre */
	if (ll_bcon_list_get_bcon_by_symbol (bcon_list, bcon->symbol)) {
		return bcon_list;
	}
	/* Añadimos la conectiva al final de la lista */
	else {
		return g_list_append (bcon_list, bcon);
	}
}


/**
 * Define las conectivas unarias según el modelo de las lógicas multivaluadas de
 * Łukasiewicz. Sólo se define una conectiva unaria, la negación (N), del
 * siguiente modo: \f$ \neg x =_{def} ( 1 - x ) \f$.
 */
void
ll_logic_set_default_ucons_lukasiewicz (LlLogic* logic)
{
	LlUCon    *ucon;
	int       *matrix;
	int       i;

	matrix = g_new (int, DIM);

	/* Negación (~, N):
	 *  ~x [Np]  =df  (1 - x)  */
	for (i = 0; i < DIM; i++) {
		matrix[i] = MAXV - i;
	}
	ucon = ll_ucon_new ("~", "N", matrix, DIM);
	logic->ucons = ll_ucon_list_append (logic->ucons, ucon);

	g_free (matrix);
}


/**
 * Define las conectivas binarias según el modelo de las lógicas multivaluadas
 * de Łukasiewicz. Las conectivas binarias que se definen son las siguientes:
 *	 - Implicación (C): \f$ x \rightarrow y =_{def} \min \{n, n - x + y\} \f$.
 *	 - Conjunción	(K): \f$ x \wedge y =_{def} \min \{x, y\} \f$.
 *	 - Disyunción	(A): \f$ x \vee y =_{def} \max \{x, y\} \f$.
 *	 - Bicondicional (E): \f$ x \leftrightarrow y =_{def} n - | x - y | \f$.
 */
void
ll_logic_set_default_bcons_lukasiewicz (LlLogic* logic)
{
	LlBCon    *bcon;
	int       **mtx;
	int       i, j;

	mtx = g_new (int*, DIM);
	for (i = 0; i < DIM; i++) {
		mtx[i] = g_new (int, DIM);
	}

	/* Implicación (->, C):
	 *  x -> y [Cxy]  =df  min {n, n - x + y}  */
	for (i = 0; i < DIM; i++) {
		for (j = 0; j < DIM; j++) {
			mtx[i][j] = MIN (MAXV, (MAXV - i + j));
		}
	}
	bcon = ll_bcon_new ("->", "C", mtx, DIM);
	logic->bcons = ll_bcon_list_append (logic->bcons, bcon);

	/* Conjunción (&, K):
	 *  x & y [Kxy]  =df  min {x, y}  */
	for (i = 0; i < DIM; i++) {
		for (j = 0; j < DIM; j++) {
			mtx[i][j] = MIN (i, j);
		}
	}
	bcon = ll_bcon_new ("&", "K", mtx, DIM);
	logic->bcons = ll_bcon_list_append (logic->bcons, bcon);

	/* Disyunción (v, A):
	 *  x v y [Axy]  =df  max {x, y}  */
	for (i = 0; i < DIM; i++) {
		for (j = 0; j < DIM; j++) {
			mtx[i][j] = MAX (i, j);
		}
	}
	bcon = ll_bcon_new ("v", "A", mtx, DIM);
	logic->bcons = ll_bcon_list_append (logic->bcons, bcon);

	/* Bicondicional (<->, E):
	 *  x <-> y [Exy]  =df  n - |x - y|  */
	for (i = 0; i < DIM; i++) {
		for (j = 0; j < DIM; j++) {
			mtx[i][j] = MAXV - abs (i - j);
		}
	}
	bcon = ll_bcon_new ("<->", "E", mtx, DIM);
	logic->bcons = ll_bcon_list_append (logic->bcons, bcon);

	g_free (mtx);
}


/**
 * Elimina de una lógica todas las apariciones de una conectiva, unaria o
 * binaria indistintamente, dada por su símbolo.
 */
void
ll_con_delete_by_symbol (LlLogic* logic, const char *symbol)
{
	LlUCon    *ucon;
	LlBCon    *bcon;

	ucon = ll_ucon_list_get_ucon_by_symbol (logic->ucons, symbol);
	if (ucon) {
		logic->ucons = g_list_remove (logic->ucons, ucon);
		ll_ucon_free (ucon);
	}
	bcon = ll_bcon_list_get_bcon_by_symbol (logic->bcons, symbol);
	if (bcon) {
		logic->bcons = g_list_remove (logic->bcons, bcon);
		ll_bcon_free (bcon, DIM);
	}
}


/**
 * Elimina de una lógica todas las apariciones de una conectiva, unaria o
 * binaria indistintamente, dada por su símbolo.
 */
void
ll_con_delete_by_symbol_pn (LlLogic* logic, char* symbol)
{
	LlUCon    *ucon;
	LlBCon    *bcon;

	ucon = ll_ucon_list_get_ucon_by_symbol (logic->ucons, symbol);
	if (ucon) {
		logic->ucons = g_list_remove (logic->ucons, ucon);
		g_free (ucon);
	}
	ucon = ll_ucon_list_get_ucon_by_symbol_pn (logic->ucons, symbol);
	if (ucon) {
		logic->ucons = g_list_remove (logic->ucons, ucon);
		g_free (ucon);
	}
	bcon = ll_bcon_list_get_bcon_by_symbol (logic->ucons, symbol);
	if (bcon) {
		logic->bcons = g_list_remove (logic->bcons, bcon);
		g_free (bcon);
	}
	bcon = ll_bcon_list_get_bcon_by_symbol_pn (logic->ucons, symbol);
	if (bcon) {
		logic->bcons = g_list_remove (logic->bcons, bcon);
		g_free (bcon);
	}
}


/**
 * Escribe en un archivo externo la matriz de una conectiva unaria en forma de
 * tabla simple. El formato pretende ser legible tanto por una persona como por
 * el ordenador, editable como texto plano e importable por el programa.
 *
 * @return 0: éxito.\n
 *				 1: error, archivo inválido.
 */
int
write_ucon_matrix (FILE *file, LlUCon* ucon, int dimension)
{
	int i;

	if (file) {
		fprintf (file, "%s\n", ucon->symbol);
		for (i = 0; i < dimension; i++) {
			fprintf (file, "%i ", ucon->matrix[i]);
		}
		fprintf (file, "\n\n");
		return 0;
	}
	else {
		return 1;
	}
}


/**
 * Escribe en un archivo externo la matriz de una conectiva binaria en forma de
 * tabla simple. El formato pretende ser legible tanto por una persona como por
 * el ordenador, editable como texto plano e importable por el programa.
 *
 * @return 0: éxito.\n
 *				 1: error, archivo inválido.
 */
int
write_bcon_matrix (FILE *file, LlBCon* bcon, int dimension)
{
	int i, j;

	if (file) {
		fprintf (file, "%s\n", bcon->symbol);
		for (i = 0; i < dimension; i++) {
			for (j = 0; j < dimension; j++)
				fprintf (file, "%i ", bcon->matrix[i][j]);
			fprintf (file, "\n");
		}
		fprintf (file, "\n");
		return 0;
	}
	else {
		return 1;
	}
}


/**
 * Escribe todas las matrices de todas las conectivas en un archivo externo en
 * forma de tabla simple. Primero se escriben las conectivas unarias y luego las
 * binarias, en el orden en que hayan sido definidas. El formato pretende ser
 * legible tanto por una persona como por el ordenador, editable como texto
 * clano e importable por el programa.
 *
 * @return 0: éxito.\n
 *				 1: error, archivo inválido.
 */
int
write_matrices (FILE *file, LlLogic* logic)
{
	GList    *iter;

	for (iter = logic->ucons; iter; iter = iter->next) {
		write_ucon_matrix (file, (LlUCon*) iter->data, DIM);
	}
	for (iter = logic->bcons; iter; iter = iter->next) {
		write_bcon_matrix (file, iter->data, DIM);
	}
}


/*
 * Imprime la matriz de una conectiva unaria en forma de tabla, marcando los
 * valores designados con asterisco.
 */
gchar*
sprint_ucon_matrix (LlUCon* ucon, LlLogic* logic)
{
	GString    *buffer;
	int         i;

	buffer = g_string_new (NULL);

	for (i = 0; i < SLEN - strlen (ucon->symbol); i++) {
		g_string_append_c (buffer, ' ');
	}
	g_string_append_printf (buffer, "%s |", ucon->symbol);

	for (i = 0; i < DIM; i++) {
		if (i >= MDV) {
			g_string_append_printf (buffer, " *%i", i);
		}
		else {
			g_string_append_printf (buffer, "  %i", i);
		}
	}
	g_string_append_c (buffer, '\n');

	for (i = 0; i <= SLEN; i++) {
		g_string_append_c (buffer, '-');
	}
	g_string_append_c (buffer, '+');
	for (i = 0; i < DIM; i++) {
		g_string_append (buffer, "---");
	}
	g_string_append (buffer, "-\n");
	for (i = 0; i <= SLEN; i++) {
		g_string_append_c (buffer, ' ');
	}
	g_string_append_c (buffer, '|');
	for (i = 0; i < DIM; i++) {
		if (ucon->matrix[i] >= MDV) {
			g_string_append_printf (buffer, " *%i", ucon->matrix[i]);
		}
		else {
			g_string_append_printf (buffer, "  %i", ucon->matrix[i]);
		}
	}
	g_string_append_c (buffer, '\n');

	return g_string_free (buffer, FALSE);
}


gchar*
sprint_ucon_matrix_hex (LlUCon* ucon, LlLogic* logic)
{
	GString    *buffer;
	int         i;

	buffer = g_string_new (NULL);

	for (i = 0; i < SLEN - strlen (ucon->symbol); i++) {
		g_string_append_c (buffer, ' ');
	}
	g_string_append_printf (buffer, "%s |", ucon->symbol);

	for (i = 0; i < DIM; i++) {
		if (i >= MDV) {
			g_string_append_printf (buffer, " *%x", i);
		}
		else {
			g_string_append_printf (buffer, "  %x", i);
		}
	}
	g_string_append_c (buffer, '\n');

	for (i = 0; i <= SLEN; i++) {
		g_string_append_c (buffer, '-');
	}
	g_string_append_c (buffer, '+');
	for (i = 0; i < DIM; i++) {
		g_string_append (buffer, "---");
	}
	g_string_append (buffer, "\n");
	for (i = 0; i <= SLEN; i++) {
		g_string_append_c (buffer, ' ');
	}
	g_string_append_c (buffer, '|');
	for (i = 0; i < DIM; i++) {
		if (ucon->matrix[i] >= MDV) {
			g_string_append_printf (buffer, " *%x", ucon->matrix[i]);
		}
		else {
			g_string_append_printf (buffer, "  %x", ucon->matrix[i]);
		}
	}
	g_string_append_c (buffer, '\n');

	return g_string_free (buffer, FALSE);
}


/*
 * Imprime la matriz de una conectiva unaria en forma de tabla, marcando los
 * valores designados con asterisco.
 */
const gchar*
sprint_ucon_matrix_pn (LlUCon* ucon, LlLogic* logic)
{
	gchar    *buffer,
	         *straux;
	int       i;

	buffer = g_new0 (gchar, 1);
	straux = g_strdup_printf ("  %s |", ucon->symbol);
	buffer = g_strconcat (buffer, straux, NULL);
	g_free (straux);

	for (i = 0; i < DIM; i++) {
		if (i >= MDV) {
			straux = g_strdup_printf (" *%i", i);
			buffer = g_strconcat (buffer, straux, NULL);
			g_free (straux);
		}
		else {
			straux = g_strdup_printf ("  %i", i);
			buffer = g_strconcat (buffer, straux, NULL);
			g_free (straux);
		}
	}

	buffer = g_strconcat (buffer, "\n----+", NULL);
	for (i = 0; i < DIM; i++) {
		buffer = g_strconcat (buffer, "---", NULL);
	}
	buffer = g_strconcat (buffer, "-\n", "    |", NULL);
	for (i = 0; i < DIM; i++) {
		if (ucon->matrix[i] >= MDV) {
			straux = g_strdup_printf (" *%i", ucon->matrix[i]);
			buffer = g_strconcat (buffer, straux, NULL);
			g_free (straux);
		}
		else {
			straux = g_strdup_printf ("  %i", ucon->matrix[i]);
			buffer = g_strconcat (buffer, straux, NULL);
			g_free (straux);
		}
	}
	buffer = g_strconcat (buffer, "\n", NULL);

	return buffer;
}


/*
 * Imprime la matriz de una conectiva binaria en forma de tabla, marcando los
 * valores designados con asterisco.
 */
gchar*
sprint_bcon_matrix_hex (LlBCon* bcon, LlLogic* logic)
{
	GString   *buffer;
	int        i, j;

	buffer = g_string_new (NULL);
	for (i = 0; i < SLEN - strlen (bcon->symbol); i++) {
		g_string_append_c (buffer, ' ');
	}
	g_string_append_printf (buffer, "%s |", bcon->symbol);
	for (i = 0; i < DIM; i++) {
		if (i >= MDV) {
			g_string_append_printf (buffer, " *%x", i);
		}
		else {
			g_string_append_printf (buffer, "  %x", i);
		}
	}
	g_string_append_c (buffer, '\n');
	for (i = 0; i <= SLEN; i++) {
		g_string_append_c (buffer, '-');
	}
	g_string_append_c (buffer, '+');
	for (i = 0; i < DIM; i++) {
		g_string_append (buffer, "---");
	}
	g_string_append (buffer, "\n");
	for (i = 0; i < DIM; i++) {
		if (i >= MDV) {
			g_string_append_printf (buffer, " *%x |", i);
		}
		else {
			g_string_append_printf (buffer, "  %x |", i);
		}

		for (j = 0; j < DIM; j++) {
			if (bcon->matrix[i][j] >= MDV) {
				g_string_append_printf (buffer, " *%x", bcon->matrix[i][j]);
			}
			else {
				g_string_append_printf (buffer, "  %x", bcon->matrix[i][j]);
			}
		}
		g_string_append_c (buffer, '\n');
	}

	return g_string_free (buffer, FALSE);
}


gchar*
sprint_bcon_matrix (LlBCon* bcon, LlLogic* logic)
{
	GString   *buffer;
	int        i, j;

	buffer = g_string_new (NULL);
	for (i = 0; i < SLEN - strlen (bcon->symbol); i++) {
		g_string_append_c (buffer, ' ');
	}
	g_string_append_printf (buffer, "%s |", bcon->symbol);
	for (i = 0; i < DIM; i++) {
		if (i >= MDV) {
			g_string_append_printf (buffer, " *%i", i);
		}
		else {
			g_string_append_printf (buffer, "  %i", i);
		}
	}
	g_string_append_c (buffer, '\n');
	for (i = 0; i <= SLEN; i++) {
		g_string_append_c (buffer, '-');
	}
	g_string_append_c (buffer, '+');
	for (i = 0; i < DIM; i++) {
		g_string_append (buffer, "---");
	}
	g_string_append (buffer, "-\n");
	for (i = 0; i < DIM; i++) {
		if (i >= MDV) {
			g_string_append_printf (buffer, " *%i |", i);
		}
		else {
			g_string_append_printf (buffer, "  %i |", i);
		}

		for (j = 0; j < DIM; j++) {
			if (bcon->matrix[i][j] >= MDV) {
				g_string_append_printf (buffer, " *%i", bcon->matrix[i][j]);
			}
			else {
				g_string_append_printf (buffer, "  %i", bcon->matrix[i][j]);
			}
		}
		g_string_append_c (buffer, '\n');
	}

	return g_string_free (buffer, FALSE);
}


/*
 * Imprime la matriz de una conectiva binaria en forma de tabla, marcando los
 * valores designados con asterisco.
 */
const gchar*
sprint_bcon_matrix_pn (LlBCon* bcon, LlLogic* logic)
{
	gchar    *buffer,
	         *straux;
	int       i, j;

	buffer = g_new0 (gchar, 1);
	straux = g_strdup_printf ("  %s |", bcon->symbol);
	buffer = g_strconcat (buffer, straux, NULL);
	g_free (straux);
	for (i = 0; i < DIM; i++)
		{
			if (i >= MDV) {
				straux = g_strdup_printf (" *%i", i);
				buffer = g_strconcat (buffer, straux, NULL);
				g_free (straux);
			}
			else {
				straux = g_strdup_printf ("  %i", i);
				buffer = g_strconcat (buffer, straux, NULL);
				g_free (straux);
			}
		}
	buffer = g_strconcat (buffer, "\n----+", NULL);
	for (i = 0; i < DIM; i++)
		buffer = g_strconcat (buffer, "---", NULL);
	buffer = g_strconcat (buffer, "-\n", NULL);
	for (i = 0; i < DIM; i++)
		{
			if (i >= MDV) {
				straux = g_strdup_printf (" *%i |", i);
				buffer = g_strconcat (buffer, straux, NULL);
				g_free (straux);
			}
			else {
				straux = g_strdup_printf ("  %i |", i);
				buffer = g_strconcat (buffer, straux, NULL);
				g_free (straux);
			}

			for (j = 0; j < DIM; j++)
				{
					if (bcon->matrix[i][j] >= MDV) {
						straux = g_strdup_printf (" *%i", bcon->matrix[i][j]);
						buffer = g_strconcat (buffer, straux, NULL);
						g_free (straux);
					}
					else {
						straux = g_strdup_printf ("  %i", bcon->matrix[i][j]);
						buffer = g_strconcat (buffer, straux, NULL);
						g_free (straux);
					}
				}
			buffer = g_strconcat (buffer, "\n", NULL);
		}

	return buffer;
}
