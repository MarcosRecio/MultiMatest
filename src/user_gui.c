/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * user_gui.c
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

#include <string.h>
#include <time.h>
#include "callbacks.h"


/*
 * Muestra un mensaje de error al usuario. Se muestra un diálogo de error y
 * se registra en la terminal también.
 */
void
dialog_error (const gchar *message)
{
	GtkWidget  *dialog;

	/* escribe el error en la terminal */
	g_warning ("%s", message);

	/* crea un diálogo modal con el mensaje de error */
	dialog = gtk_message_dialog_new (NULL,
	                                 GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
	                                 GTK_MESSAGE_ERROR,
	                                 GTK_BUTTONS_OK,
	                                 "%s", message);

	gtk_window_set_title (GTK_WINDOW (dialog), "Error!");
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}


/*
 * Modo interactivo con interfaz gráfica.
 */
gint
mode_gui (int argc, char *argv[], Work* work)
{
	GtkBuilder             *builder;
	GtkSettings            *settings;
	GError                 *gerr = NULL;
	PangoFontDescription   *font_desc;
	MaTestGUI              *gui;
	GtkWidget              *b_con;
	GList                  *iter;
	int                     initdim = work->DIM,
	                        initmdv = work->MDV;
	gchar                  *label;
	EvaluationStyle         init_ev_style = work->evaluation_style;

	/* asigna memoria para el registro MaTestGUI */
	gui = g_new (MaTestGUI, 1);

	gui->work = work;

	/* inicializamos las librerías GTK+ */
	gtk_init (&argc, &argv);

	/* usamos GtkBuilder para contruir la interfaz desde el archivo XML */
	builder = gtk_builder_new ();
	if (gtk_builder_add_from_file (builder, UI_FILE, &gerr) == 0) {
		dialog_error (gerr->message);
		g_error_free (gerr);
		g_object_unref (G_OBJECT (builder));
		return 1;  /* error cargando la interfaz */
	}

	/* obtenemos los widgets que van a ser referenciados a llamadas */
	gui->window = GTK_WIDGET (gtk_builder_get_object (builder, "window"));
	gui->statusbar = GTK_WIDGET (gtk_builder_get_object (builder, "statusbar"));
	gui->text_view = GTK_WIDGET (gtk_builder_get_object (builder, "text_view"));
	gui->textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (gui->text_view));
	gui->spin_dimension = GTK_WIDGET (gtk_builder_get_object (builder, "spin_dimension"));
	gui->spin_mdv = GTK_WIDGET (gtk_builder_get_object (builder, "spin_mdv"));
	gui->entry_formula = GTK_WIDGET (gtk_builder_get_object (builder, "entry_formula"));
	gui->label_formula = GTK_LABEL (gtk_builder_get_object (builder, "label_formula"));
	gui->hb_ucons = GTK_WIDGET (gtk_builder_get_object (builder, "hb_ucons"));
	gui->hb_bcons = GTK_WIDGET (gtk_builder_get_object (builder, "hb_bcons"));
	gui->m_view_all = GTK_WIDGET (gtk_builder_get_object (builder, "m_view_all"));
	gui->m_view_desig = GTK_WIDGET (gtk_builder_get_object (builder, "m_view_desig"));
	gui->m_view_notdesig = GTK_WIDGET (gtk_builder_get_object (builder, "m_view_notdesig"));

	/* ajustamos la fuente del área de texto */
	font_desc = pango_font_description_from_string ("monospace 10");
	gtk_widget_modify_font (gui->text_view, font_desc);
	pango_font_description_free (font_desc);

	/* inicializa los rangos de la dimensión y el mínimo valor designado así como
	 * sus valores iniciales */
	gtk_spin_button_set_range (GTK_SPIN_BUTTON (gui->spin_dimension),
	                           2, 16);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (gui->spin_dimension),
	                           initdim);
	gtk_spin_button_set_range (GTK_SPIN_BUTTON (gui->spin_mdv),
	                           1, gui->work->MAXV);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (gui->spin_mdv),
	                           initmdv);

	/* establece el estilo de evaluación inicial */
	switch (init_ev_style) {
		case ALL:
			gtk_menu_item_activate (GTK_MENU_ITEM (gui->m_view_all));
			break;
		case DESIGNATED:
			gtk_menu_item_activate (GTK_MENU_ITEM (gui->m_view_desig));
			break;
		case NOT_DESIGNATED:
			gtk_menu_item_activate (GTK_MENU_ITEM (gui->m_view_notdesig));
			break;
	}

	/* Establece un mensaje inicial para la barra de estado */
	gtk_statusbar_push (GTK_STATUSBAR (gui->statusbar),
	                    gtk_statusbar_get_context_id (GTK_STATUSBAR (gui->statusbar),
	                                                  "General"),
	                    "Preparado");
	/* Muestra las imágenes en los botones */
	g_object_set (gtk_settings_get_default (),
	              "gtk-button-images",
	              TRUE, NULL);
	/* conecta las señales, pasamos el registro MaTestGUI como datos de usuario */
	gtk_builder_connect_signals (builder, gui);
	/* libera la memoria del objeto GtkBuilder */
	g_object_unref (G_OBJECT (builder));

	/* conecta la señal para el cierre de la aplicación con confirmación */
	g_signal_connect (G_OBJECT (gui->window), "delete-event",
	                  G_CALLBACK (on_window_delete_event), gui);

	/* define las conectivas por defecto */
	ll_logic_set_default_ucons_lukasiewicz (gui->work->logic);
	ll_logic_set_default_bcons_lukasiewicz (gui->work->logic);

	/* crea los botones para las conectivas */
	for (iter = gui->work->logic->ucons; iter; iter = iter->next)	{
		label = g_strdup_printf ("%s", ((LlUCon*) iter->data)->symbol);
		b_con = gtk_button_new_with_label (label);
		g_free (label);
		g_signal_connect (G_OBJECT (b_con), "clicked",
	                    G_CALLBACK (on_b_ucon_clicked), (gpointer) gui);
	  gtk_button_set_relief (b_con, GTK_RELIEF_NONE);
	  gtk_container_add (GTK_CONTAINER (gui->hb_ucons), b_con);
		gtk_widget_show (b_con);
	}
	for (iter = gui->work->logic->bcons; iter; iter = iter->next)	{
		label = g_strdup_printf ("%s", ((LlUCon*) iter->data)->symbol);
		b_con = gtk_button_new_with_label (label);
		g_free (label);
		g_signal_connect (G_OBJECT (b_con), "clicked",
		                  G_CALLBACK (on_b_bcon_clicked), gui);
		gtk_button_set_relief (b_con, GTK_RELIEF_NONE);
		gtk_container_add (GTK_CONTAINER (gui->hb_bcons), b_con);
		gtk_widget_show (b_con);
	}

	/* Muestra la ventana */
	gtk_widget_show (gui->window);

	/* Entra en el bucle principal GTK+ */
	gtk_main ();

	/* Liberamos la memoria usada por el registro MaTestGUI */
	g_free (gui);

	return 0;
}


/*
 * Diálogo Acerca de
 */
gboolean
dialog_about (void)
{
	GtkWidget     *dialog;
	GtkBuilder    *builder;
	GError        *err = NULL;

	/* usamos GtkBuilder para contruir el díalogo desde su archivo XML */
	builder = gtk_builder_new ();
	if (gtk_builder_add_from_file (builder, ABOUT_DIALOG_FILE, &err) == 0) {
		dialog_error (err->message);
		g_error_free (err);
		g_object_unref (G_OBJECT (builder));
		return FALSE;
	}

	dialog = GTK_WIDGET (gtk_builder_get_object (builder, "aboutdialog"));
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
	g_object_unref (G_OBJECT (builder));

	return TRUE;
}


/*
 * Guardar matrices a archivo
 */
gint
write_matrices_gui (MaTestGUI *gui)
{
	GtkWidget   *chooser;
	gchar       *filename;
	struct tm   *local;
  time_t       t;
	FILE        *file;

	chooser = gtk_file_chooser_dialog_new ("Guardar matrices",
	                                       GTK_WINDOW (gui->window),
	                                       GTK_FILE_CHOOSER_ACTION_SAVE,
	                                       GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
	                                       GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
	                                       NULL);
	gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (chooser), TRUE);
	if (gtk_dialog_run (GTK_DIALOG (chooser)) == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
		file = fopen (filename, "w");
		if (file) {
			t = time (NULL);
			local = localtime (&t);
			fprintf (file, "# Generado por MaTest-%s en %s\n", VERSION, asctime (local));
			fprintf (file, "MDV %u\n\n", gui->work->MDV);
			write_matrices (file, gui->work->logic);
			fclose (file);
		}
		g_free (filename);
	}

	gtk_widget_destroy (chooser);
	return 0;
}


/* FIXME Estas dos funciones se pueden reducir a una sóla */
void
button_ucon_add (MaTestGUI *gui, const char *symbol)
{
	GtkWidget     *b_ucon;

	/* Ponemos la etiqueta con el símbolo de la conectiva */
	b_ucon = gtk_button_new_with_label (symbol);
	g_signal_connect (G_OBJECT (b_ucon), "clicked",
		                G_CALLBACK (on_b_ucon_clicked), gui);
	gtk_container_add (GTK_CONTAINER (gui->hb_ucons), b_ucon);
	gtk_widget_show (b_ucon);
}


void
button_bcon_add (MaTestGUI *gui, const char *symbol)
{
	GtkWidget     *b_bcon;

	/* Ponemos la etiqueta con el símbolo de la conectiva */
	b_bcon = gtk_button_new_with_label (symbol);
	g_signal_connect (G_OBJECT (b_bcon), "clicked",
		                G_CALLBACK (on_b_bcon_clicked), gui);
	gtk_container_add (GTK_CONTAINER (gui->hb_bcons), b_bcon);
	gtk_widget_show (b_bcon);
}


void
button_con_delete_by_symbol (GtkButton *button, const gchar *symbol)
{
	gchar     *label;

	label = (gchar*) gtk_button_get_label (button);
	if (strcmp (label, symbol) == 0) {
		gtk_widget_destroy (GTK_WIDGET (button));
	}
}


gint
dialog_ucon_new (MaTestGUI *gui)
{
	GtkDialog       *dialog;
	GtkWidget       *content,
									*table,
									*input_symbol,
									*label_col,
									*spinvalue;
	GtkAdjustment   **values;
	LlUCon          *ucon;
	gint             i;
	int             *matrix;
	gchar           *label;

	values = (GtkAdjustment**) g_new (GtkAdjustment, gui->work->DIM);
	matrix = g_new0 (int, gui->work->DIM);

	dialog = GTK_DIALOG (gtk_dialog_new_with_buttons ("Nueva conectiva unaria",
	                                                  GTK_WINDOW (gui->window),
	                                                  GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
	                                                  GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
	                                                  GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
	                                                  NULL));

	content = gtk_dialog_get_content_area (dialog);
	table = gtk_table_new (2, gui->work->DIM + 1, FALSE);
	gtk_container_add (GTK_CONTAINER (content), table);
	input_symbol = gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (input_symbol), SLEN);
	gtk_entry_set_width_chars (GTK_ENTRY (input_symbol), SLEN);
	gtk_table_attach_defaults (GTK_TABLE (table), input_symbol,
	                           0, 1,
	                           0, 1);
	for (i = 0; i < gui->work->DIM; i++) {
		label = g_strdup_printf ("%x", i);
		label_col = gtk_label_new (label);
		g_free (label);
		gtk_table_attach_defaults (GTK_TABLE (table), label_col,
		                           i + 1, i + 2,
		                           0, 1);
		spinvalue = gtk_spin_button_new_with_range (0, gui->work->MAXV, 1);
		gtk_spin_button_set_digits (GTK_SPIN_BUTTON (spinvalue), 0);
		gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinvalue), TRUE);
		values[i] = gtk_spin_button_get_adjustment (GTK_SPIN_BUTTON (spinvalue));
		gtk_table_attach_defaults (GTK_TABLE (table), spinvalue,
		                           i + 1, i + 2,
		                           1, 2);
	}
	gtk_widget_show_all (content);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
		label = (gchar*) gtk_entry_get_text (GTK_ENTRY (input_symbol));
		if (ll_ucon_list_get_ucon_by_symbol (gui->work->logic->ucons, label) == NULL &&
		    ll_bcon_list_get_bcon_by_symbol (gui->work->logic->bcons, label) == NULL) {
			for (i = 0; i < gui->work->DIM; i++) {
				matrix[i] = (int) gtk_adjustment_get_value (values[i]);
			}
			ucon = ll_ucon_new (label, label, matrix, gui->work->DIM);
			gui->work->logic->ucons = ll_ucon_list_append (gui->work->logic->ucons, ucon);
			button_ucon_add (gui, label);
		}
	}
	gtk_widget_destroy (GTK_WIDGET (dialog));

	return 0;
}


gint
dialog_ucon_edit (MaTestGUI *gui, const gchar *symbol)
{
	GtkDialog       *dialog;
	GtkWidget       *content,
	                *table,
	                *label_symb,
	                *label_col,
	                *b_delete,
	                **spinvalues;
	LlUCon          *ucon;
	GList           *ucon_bs;
	gchar           *label,
	                *esc_symbol;
	gint             response,
	                 i;

	ucon = ll_ucon_list_get_ucon_by_symbol (gui->work->logic->ucons, symbol);
	spinvalues = (GtkWidget**) g_new (GtkWidget, gui->work->DIM);

	label = g_strdup_printf ("Editar %s", symbol);
	dialog = GTK_DIALOG (gtk_dialog_new_with_buttons (label,
	                                                  GTK_WINDOW (gui->window),
	                                                  GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
	                                                  GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
	                                                  "Borrar", 1,
	                                                  GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
	                                                  NULL));
	g_free (label);

	content = gtk_dialog_get_content_area (dialog);
	table = gtk_table_new (2, gui->work->DIM + 1, TRUE);
	gtk_container_add (GTK_CONTAINER (content), table);
	label_symb = gtk_label_new (NULL);
	esc_symbol = g_markup_escape_text (symbol, -1);
	label = g_strdup_printf ("<b>%s</b>", esc_symbol);
	g_free (esc_symbol);
	gtk_label_set_markup (GTK_LABEL (label_symb), label);
	g_free (label);
	gtk_table_attach_defaults (GTK_TABLE (table), label_symb,
	                           0, 1,
	                           0, 1);

	for (i = 0; i < gui->work->DIM; i++) {
		label = g_strdup_printf ("%x", i);
		label_col = gtk_label_new (label);
		g_free (label);
		gtk_table_attach_defaults (GTK_TABLE (table), label_col,
		                           i + 1, i + 2,
		                           0, 1);
		spinvalues[i] = gtk_spin_button_new_with_range (0, gui->work->MAXV, 1);
		gtk_spin_button_set_digits (GTK_SPIN_BUTTON (spinvalues[i]), 0);
		gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinvalues[i]), TRUE);
		gtk_spin_button_set_value (GTK_SPIN_BUTTON (spinvalues[i]),
				                       ucon->matrix[i]);
		gtk_table_attach_defaults (GTK_TABLE (table), spinvalues[i],
		                           i + 1, i + 2,
		                           1, 2);
	}

	gtk_widget_show_all (content);

	response = gtk_dialog_run (GTK_DIALOG (dialog));
	switch (response) {
		case GTK_RESPONSE_ACCEPT :
			for (i = 0; i < gui->work->DIM; i++) {
				ucon->matrix[i] = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (spinvalues[i]));
			}
			break;
		case 1 :
			ll_con_delete_by_symbol (gui->work->logic, (gchar*) symbol);
			for (ucon_bs = gtk_container_get_children (GTK_CONTAINER (gui->hb_ucons));
			     ucon_bs;
			     ucon_bs = ucon_bs->next) {
				button_con_delete_by_symbol (GTK_BUTTON (ucon_bs->data), symbol);
			}
			g_list_free (ucon_bs);
			break;
	}

	gtk_widget_destroy (GTK_WIDGET (dialog));
	return 0;
}


gint
dialog_bcon_new (MaTestGUI *gui)
{
	GtkDialog       *dialog;
	GtkWidget       *content,
	                *table,
	                *input_symbol,
	                *label_row, *label_col,
	                *spinvalue;
	GtkAdjustment   ***values;
	LlBCon          *bcon;
	gint             i, j;
	int             **matrix;
	gchar           *label;

	values = (GtkAdjustment***) g_new (GtkAdjustment*, gui->work->DIM);
	for (i = 0; i < gui->work->DIM; i++) {
		values[i] = (GtkAdjustment**) g_new (GtkAdjustment, gui->work->DIM);
	}
	matrix = g_new (int*, gui->work->DIM);
	for (i = 0; i < gui->work->DIM; i++) {
		matrix[i] = g_new0 (int, gui->work->DIM);
	}

	dialog = GTK_DIALOG (gtk_dialog_new_with_buttons ("Nueva conectiva binaria",
	                                                  GTK_WINDOW (gui->window),
	                                                  GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
	                                                  GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
	                                                  GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
	                                                  NULL));

	content = gtk_dialog_get_content_area (dialog);
	table = gtk_table_new (gui->work->DIM + 1, gui->work->DIM + 1, FALSE);
	gtk_container_add (GTK_CONTAINER (content), table);
	input_symbol = gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (input_symbol), SLEN);
	gtk_entry_set_width_chars (GTK_ENTRY (input_symbol), SLEN);
	gtk_table_attach_defaults (GTK_TABLE (table), input_symbol,
	                           0, 1,
	                           0, 1);
	for (i = 0; i < gui->work->DIM; i++) {
		label = g_strdup_printf ("%x", i);
		label_row = gtk_label_new (label);
		gtk_table_attach_defaults (GTK_TABLE (table), label_row,
		                           0, 1,
		                           i + 1, i + 2);
		label_col = gtk_label_new (label);
		gtk_table_attach_defaults (GTK_TABLE (table), label_col,
			                         i + 1, i + 2,
			                         0, 1);
		g_free (label);
		for (j = 0; j < gui->work->DIM; j++) {
			spinvalue = gtk_spin_button_new_with_range (0, gui->work->MAXV, 1);
			gtk_spin_button_set_digits (GTK_SPIN_BUTTON (spinvalue), 0);
			gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinvalue), TRUE);
			values[i][j] = gtk_spin_button_get_adjustment (GTK_SPIN_BUTTON (spinvalue));
			gtk_table_attach_defaults (GTK_TABLE (table), spinvalue,
			                           j + 1, j + 2,
		  		                       i + 1, i + 2);
		}
	}
	gtk_widget_show_all (content);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
		label = (gchar*) gtk_entry_get_text (GTK_ENTRY (input_symbol));
		if (ll_ucon_list_get_ucon_by_symbol (gui->work->logic->ucons, label) == NULL &&
		    ll_bcon_list_get_bcon_by_symbol (gui->work->logic->bcons, label) == NULL) {
			for (i = 0; i < gui->work->DIM; i++) {
				for (j = 0; j < gui->work->DIM; j++) {
					matrix[i][j] = (int) gtk_adjustment_get_value (values[i][j]);
				}
			}
			bcon = ll_bcon_new (g_ascii_strup (label, -1), g_ascii_strup (label, -1), matrix, gui->work->DIM);
			gui->work->logic->bcons = ll_bcon_list_append (gui->work->logic->bcons, bcon);
			button_bcon_add (gui, label);
		}
	}
	gtk_widget_destroy (GTK_WIDGET (dialog));

	return 0;
}


gint
dialog_bcon_edit (MaTestGUI *gui, const gchar *symbol)
{
	GtkDialog    *dialog;
	GtkWidget    *content,
	             *table,
	             *label_symb,
	             *label_row, *label_col,
	             ***spinvalues;
	LlBCon       *bcon;
	GList        *bcon_bs;
	gchar        *label,
	             *esc_symbol;
	gint          response,
	              i, j;

	bcon = ll_bcon_list_get_bcon_by_symbol (gui->work->logic->bcons, symbol);
	spinvalues = (GtkWidget***) g_new (GtkWidget*, gui->work->DIM);
	for (i = 0; i < gui->work->DIM; i++) {
		spinvalues[i] = (GtkWidget**) g_new (GtkWidget, gui->work->DIM);
	}

	label = g_strdup_printf ("Editar %s", symbol);
	dialog = GTK_DIALOG (gtk_dialog_new_with_buttons (label,
	                                                  GTK_WINDOW (gui->window),
	                                                  GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
	                                                  GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
	                                                  "Borrar", 1,
	                                                  GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
	                                                  NULL));
	g_free (label);

	content = gtk_dialog_get_content_area (dialog);
	table = gtk_table_new (gui->work->DIM + 1, gui->work->DIM + 1, TRUE);
	gtk_container_add (GTK_CONTAINER (content), table);
	label_symb = gtk_label_new (NULL);
	esc_symbol = g_markup_escape_text (symbol, -1);
	label = g_strdup_printf ("<b>%s</b>", esc_symbol);
	g_free (esc_symbol);
	gtk_label_set_markup (GTK_LABEL (label_symb), label);
	g_free (label);
	gtk_table_attach_defaults (GTK_TABLE (table), label_symb,
	                           0, 1,
	                           0, 1);

	for (i = 0; i < gui->work->DIM; i++) {
		label = g_strdup_printf ("%x", i);
		label_row = gtk_label_new (label);
		gtk_table_attach_defaults (GTK_TABLE (table), label_row,
		                           0, 1,
		                           i + 1, i + 2);
		label_col = gtk_label_new (label);
		gtk_table_attach_defaults (GTK_TABLE (table), label_col,
			                         i + 1, i + 2,
			                         0, 1);
		g_free (label);
		for (j = 0; j < gui->work->DIM; j++) {
			spinvalues[i][j] = gtk_spin_button_new_with_range (0, gui->work->MAXV, 1);
			gtk_spin_button_set_digits (GTK_SPIN_BUTTON (spinvalues[i][j]), 0);
			gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinvalues[i][j]), TRUE);
			gtk_spin_button_set_value (GTK_SPIN_BUTTON (spinvalues[i][j]),
					                       bcon->matrix[i][j]);
			gtk_table_attach_defaults (GTK_TABLE (table),
			                           spinvalues[i][j],
			                           j + 1, j + 2,
				                         i + 1, i + 2);
		}
	}
	gtk_widget_show_all (content);

	response = gtk_dialog_run (GTK_DIALOG (dialog));
	if (response == GTK_RESPONSE_ACCEPT) {
		for (i = 0; i < gui->work->DIM; i++) {
			for (j = 0; j < gui->work->DIM; j++) {
				bcon->matrix[i][j] = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (spinvalues[i][j]));
			}
		}
	}
	/* Si se pulsa el botón Borrar eliminamos la conectiva y su botón */
	else if (response == 1) {
		ll_con_delete_by_symbol (gui->work->logic, symbol);
		for (bcon_bs = gtk_container_get_children (GTK_CONTAINER (gui->hb_bcons));
		     bcon_bs;
		     bcon_bs = bcon_bs->next) {
			button_con_delete_by_symbol (GTK_BUTTON (bcon_bs->data), symbol);
		}
		g_list_free (bcon_bs);
	}
	gtk_widget_destroy (GTK_WIDGET (dialog));

	return 0;
}


gchar*
get_current_evaluating_formula_pn (char formula[], LlLogic* logic)
{
	int        i;
	char       symbol[2];
	GString    *text;

	text = g_string_new ("");

	for (i = 0; i < (int) strlen (formula); i++)
		{
			//sprintf (symbol, "%c", formula[i]);
			if (ll_symbol_pn_get_type (formula[i], logic) == LL_SYMBOL_VAR)
				g_string_append_printf (text, "%x", ll_var_get_value (ll_var_list_get_var_by_symbol (logic->vars, symbol)));
			else
				g_string_append_printf (text, "%c", formula[i]);
		}

	return g_string_free (text, FALSE);
}
