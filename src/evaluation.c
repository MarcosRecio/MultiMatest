/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * evaluation.c
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


/** @file evaluation.c
 *
 * El código de este archivo se encarga de realizar la evaluación según los
 * elementos que se hayen definidos.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "MaTest.h"


gchar*
sprint_varlist (LlLogic *logic)
{
	LlVar   *var;
	GList   *iter;
	gchar   *buffer,
	        *straux = NULL;
	gint     length;

	length = 2 * ll_var_list_length (logic->vars) + 1;
	buffer = g_new0 (gchar, length);

	for (iter = logic->vars; iter; iter = iter->next) {
		var = (LlVar*) iter->data;
		straux = g_strdup_printf ("%s ", var->symbol);
		g_strlcat (buffer, straux, length);
		g_free (straux);
	}

	return buffer;
}


gchar*
sprint_varlist_values (GList* var_list)
{
	LlVar    *var;
	GList    *iter = NULL;
	gchar    *buffer,
	         *straux;
	gint      length;

	length = 2 * ll_var_list_length (var_list) + 1;
	buffer = g_new0 (gchar, length);

	for (iter = var_list; iter; iter = iter->next) {
		var = (LlVar*) iter->data;
		straux = g_strdup_printf ("%x ", var->value);
		g_strlcat (buffer, straux, length);
		g_free (straux);
	}

	return buffer;
}


gchar*
counter (Work* work, gchar* (*single_action) (Work*))
{
	GString    *buffer;
	gchar      *straux;
	LlVar      *var;
	GList      *position;
	guint       dimension;

	buffer = g_string_new (NULL);

	/* Condición inicial: todos los valores a 0 */
	g_list_foreach (work->logic->vars, (GFunc) ll_var_set_value, 0);

	/* Primera acción en base al estado inicial */
	straux = single_action (work);
	g_string_append (buffer, straux);
	g_free (straux);

	/* El contador */
	position = work->logic->vars;
	dimension = work->MAXV;
	do {
		var = position->data;
		if (ll_var_get_value (var) < dimension) {
			var->value++;
			position = work->logic->vars;
			straux = single_action (work);
			g_string_append (buffer, straux);
			g_free (straux);
		}
		else {
			var->value = 0;
			position = position->next;
		}
	}
	while (position);

	return g_string_free (buffer, FALSE);
}


/*
 * Devuelve una cadena con la fórmula bien formada que se pasa como argumento,
 * escrita en notación polaca y cambiando las variables y conectivas por los
 * valores que correspondan.
 */
gchar*
sprint_formula_valuation (LlWFF *wff, LlLogic* logic)
{
	GString    *buffer;
	gchar      *straux;

	buffer = g_string_new (NULL);

	if (!wff) {
		return g_string_free (buffer, FALSE);
	}

	straux = g_strdup_printf ("%x", wff->value);
	g_string_append (buffer, straux);
	g_free (straux);

	straux = sprint_formula_valuation (wff->prearg, logic);
	g_string_append (buffer, straux);
	g_free (straux);

	straux = sprint_formula_valuation (wff->postarg, logic);
	g_string_append (buffer, straux);
	g_free (straux);

	return g_string_free (buffer, FALSE);
}


/*
 * Devuelve una cadena con la fórmula bien formada que se pasa como argumento,
 * escrita en notación polaca y cambiando las variables y conectivas por los
 * valores que correspondan.
 */
gchar*
sprint_formula_valuation_pn (LlWFF *wff, LlLogic* logic)
{
	GString    *buffer;
	gchar      *straux;

	buffer = g_string_new (NULL);

	if (!wff) {
		return g_string_free (buffer, FALSE);
	}

	straux = g_strdup_printf ("%x", wff->value);
	g_string_append (buffer, straux);
	g_free (straux);

	straux = sprint_formula_valuation_pn (wff->prearg, logic);
	g_string_append (buffer, straux);
	g_free (straux);

	straux = sprint_formula_valuation_pn (wff->postarg, logic);
	g_string_append (buffer, straux);
	g_free (straux);

	return g_string_free (buffer, FALSE);
}


gchar*
sprint_single_eval (Work* work)
{
	GString    *buffer;
	gchar      *straux;
	int         i;

	buffer = g_string_new (NULL);

	ll_wff_valuate (work->wff, work->logic);
	work->valuations_all++;
	if (work->wff->value >= work->MDV) {
		work->valuations_desig++;
	}

	if (work->evaluation_style == DESIGNATED &&
	    work->wff->value < work->MDV) {
		return g_string_free (buffer, FALSE);
	}
	else if (work->evaluation_style == NOT_DESIGNATED &&
	         work->wff->value >= work->MDV) {
		return g_string_free (buffer, FALSE);
	}
	else {
		straux = sprint_varlist_values (work->logic->vars);
		g_string_append (buffer, straux);
		g_free (straux);

		g_string_append_c (buffer, ' ');
		straux = ll_wff_sprint_formula_values (work->wff, work->logic);
		/* Quitamos los paréntesis exteriores de la fórmula, si los hay */
		if (straux[0] == '(') {
			for (i = 0; i < strlen (straux); i++) {
				straux[i] = straux[i + 1];
			}
			if (straux[strlen (straux) - 1] == ')') {
				straux[strlen (straux) - 1] = '\0';
			}
		}
		g_string_append (buffer, straux);
		g_free (straux);

		if (work->wff->value >= work->MDV) {
			straux = g_strdup_printf ("  %x*\n", work->wff->value);
		}
		else {
			straux = g_strdup_printf ("  %x\n", work->wff->value);
		}
		g_string_append (buffer, straux);
		g_free (straux);
	}

	return g_string_free (buffer, FALSE);
}


gchar*
sprint_statistics (Work *work)
{
	GString    *buffer;
	gchar      *straux;

	buffer = g_string_new (NULL);

	if (work->evaluation_style == ALL || work->evaluation_style == DESIGNATED) {
		straux = g_strdup_printf ("%i/%i valores designados.\n",
		                          work->valuations_desig, work->valuations_all);
		g_string_append (buffer, straux);
		g_free (straux);
	}
	else {
		straux = g_strdup_printf ("%i/%i valores no designados.\n",
		                          work->valuations_all - work->valuations_desig,
		                          work->valuations_all);
		g_string_append (buffer, straux);
		g_free (straux);
	}

	if (work->valuations_desig == work->valuations_all) {
		g_string_append (buffer, "TAUTOLOGÍA.\n");
	}
	else if (work->valuations_desig == 0) {
		g_string_append (buffer, "CONTRADICCIÓN.\n");
	}
	else {
		g_string_append (buffer, "Se FALSA la fórmula.\n");
	}

	return g_string_free (buffer, FALSE);
}


gchar*
sprint_evaluation (Work* work)
{
	GString    *buffer;
	gchar      *straux;
	LlVar      *var;
	int         i;

	buffer = g_string_new (NULL);

	if (!work->wff) {
		g_string_append (buffer, "No se ha introducido\nninguna fórmula.\n");
		return (g_string_free (buffer, FALSE));
	}

	work->valuations_all = 0;
	work->valuations_desig = 0;

	/* Imprime una cabecera con las variables en uso y la fórmula */
	straux = sprint_varlist (work->logic);
	g_string_append_printf (buffer, "%s", straux);
	g_free (straux);
	g_string_append_printf (buffer, " %s\n", work->formula);
	for (i = 0; i < 2 * ll_var_list_length (work->logic->vars) - 1; i++) {
		g_string_append_c (buffer, '-');
	}
	g_string_append (buffer, "  ");
	for (i = 0; i < strlen (work->formula); i++) {
		g_string_append_c (buffer, '-');
	}
	g_string_append_c (buffer, '\n');

	/* Aplicamos el algoritmo contador */
	straux = counter (work, sprint_single_eval);
	g_string_append_printf (buffer, "%s", straux);
	g_free (straux);

	/* Imprime las estadísticas */
	g_string_prepend_c (buffer, '\n');
	straux = sprint_statistics (work);
	g_string_prepend (buffer, straux);
	g_free (straux);

	return (g_string_free (buffer, FALSE));
}
