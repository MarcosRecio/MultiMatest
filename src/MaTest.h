/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * MaTest.h
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


/** @file MaTest.h
 *
 * Este archivo contiene declaraciones del preprocesador, definición de tipos
 * de datos y los prototipos de las funciones comunes a todo el programa.
 */

#ifndef __MATEST_H__
#define __MATEST_H__


/*
***	Declaraciones del preprocesador
 */

#include <stdbool.h>
#include <libintl.h>
#include <gtk/gtk.h>
#include "logics/logics.h"

#define VERSION "1.3"
#define MAX_FORMULA_LENGHT BUFSIZ


/*
***	Definición de tipos de datos
 */

// TODO Añadir el modo de comprobación de una regla
/** @enum Enumeración con los tipos de evaluación. */
typedef enum
{
	ALL,             /**< Se muestran todos los valores. */
	DESIGNATED,      /**< Se muestran sólo los valores designados. */
	NOT_DESIGNATED,  /**< Se muestran sólo los valores no designados. */
	RULE
}
EvaluationStyle;


//TODO Sólo contar las evaluaciones designadas
/** @struct
 * Registro general con todos los datos necesarios para la evaluación, esto es,
 * contiene el trabajo definido actualmente tal y como se evaluará.
 */
typedef struct _Work Work;
struct _Work
{
	LlLogic*           logic;    /**< Una lógica definida como se quiera. */
	char               formula[MAX_FORMULA_LENGHT];  /**< Fórmula en notación polaca. */
	LlWFF             *wff;      /**< Una fórmula bien formada como estructura en árbol. */
	EvaluationStyle    evaluation_style;  /**< El tipo de evaluación a realizar. */
	int                valuations_all,
	                   valuations_desig;
	bool               logic_modified;
};


/*
 * Registro con los widgets y demás datos de la interfaz gráfica a los que se
 * necesita acceder.
 */
typedef struct
{
	GtkWidget      *window,
	               *statusbar,
	               *text_view,
	               *progressbar,
	               *spin_dimension, *spin_mdv,
	               *entry_formula,
	               *hb_ucons, *hb_bcons,
	               *m_view_all, *m_view_desig, *m_view_notdesig;
	GtkTextBuffer  *textbuffer;
	GtkLabel       *label_formula;
	Work           *work;
}
MaTestGUI;


/*
***	Prototipos de las funciones
 */

/* Modos interactivos */
int        mode_tui                     (Work        *work);
int        mode_gui                     (int          argc,
                                         char       **argv,
                                         Work        *work);

/* Manejo de conectivas */
void       tui_ucon_add_custom          (LlLogic     *logic,
                                         const char *symbol);
void       tui_bcon_add_custom          (LlLogic     *logic,
                                         const char *symbol);
gchar*     sprint_ucon_matrix           (LlUCon      *ucon,
                                         LlLogic     *logic);
gchar*     sprint_bcon_matrix           (LlBCon      *bcon,
                                         LlLogic     *logic);
void       print_matrices               (LlLogic     *logic);
int        write_ucon_matrix            (FILE        *file,
                                         LlUCon      *ucon,
                                         int          dimension);
int        write_bcon_matrix            (FILE        *file,
                                         LlBCon      *bcon,
                                         int          dimension);
int        write_matrices               (FILE        *file,
                                         LlLogic     *logic);

/* Manejo de variables */
void       ll_logic_add_formula_vars    (LlLogic     *logic,
                                         const char *formula);

/* Funciones relacionadas con la evaluación */
void       print_ll_wff_get_value       (const char *formula,
                                         LlLogic     *logic);
void       evaluation                   (FILE        *output,
                                         Work        *work);

/* Funciones del modo texto */
char       readin                       (char        *str,
                                         const char  *pattern);
void       screen_clear                 (void);
void       make_pause                   (void);
void       menu_usage                   (void);
void       menu_version                 (void);
void       menu_header                  (void);
void       menu_info                    (Work        *work);
void       menu_options                 (void);
void       menu_init                    (void);
void       menu_dimension               (void);
void       menu_about                   (void);
void       menu_help                    (void);
void       menu_index                   (Work        *work);

/* Funciones del modo gráfico. */
gboolean   init_gui                     (MaTestGUI     *gui);
void       dialog_error                 (const gchar   *message);
gchar*     show_matrices_gui            (LlLogic       *logic);
gchar*     evaluation_gui               (MaTestGUI     *gui);
gint       dialog_ucon_new              (MaTestGUI     *gui);
gint       dialog_ucon_edit             (MaTestGUI     *gui,
                                         const gchar  *symbol);
gint       dialog_bcon_new              (MaTestGUI     *gui);
gint       dialog_bcon_edit             (MaTestGUI     *gui,
                                         const gchar  *symbol);


#endif /* __MATEST_H__ */
