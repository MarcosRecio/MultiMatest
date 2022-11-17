/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * callbacks.c
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
#include "callbacks.h"


GtkResponseType
confirm_quit_dialog()
{
	GtkWidget          *dialog;
	GtkResponseType     response;

	/* Crea el diálogo */
	dialog = gtk_message_dialog_new (NULL,
	                                 GTK_DIALOG_DESTROY_WITH_PARENT,
	                                 GTK_MESSAGE_QUESTION,
	                                 GTK_BUTTONS_YES_NO,
	                                 "Se ha modificado la lógica actual, ¿desea salir sin guardar los cambios?");

	gtk_window_set_title(GTK_WINDOW(dialog), "Cambios sin guardar");

	/* Ejecuta el diálogo */
	response = gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);

	return response;
}


gboolean
on_window_delete_event (GtkWidget *widget, GdkEvent *event, MaTestGUI *gui)
{
	if (gui->work->logic_modified == TRUE
	    && confirm_quit_dialog() == GTK_RESPONSE_NO) {
		return TRUE;   /* no salir */
	}
	else {
		return FALSE;  /* salir */
	}
}


//void
//on_window_destroy (GObject   *object, MaTestGUI *gui)
//{
	//gtk_main_quit ();
//}


/* Menú > Archivo > Salir : sale del programa. */
void
on_m_file_quit_activate (GObject   *object, MaTestGUI *gui)
{

}


/* Botón evaluar */
void
on_b_evaluate_clicked (GObject   *object, MaTestGUI *gui)
{
	gchar    *buffer;

	buffer = (gchar*) sprint_evaluation (gui->work);
	/* Quitamos el último salto de línea */
	buffer[strlen (buffer) - 1] = '\0';
	gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gui->textbuffer), buffer, -1);
	g_free (buffer);
}


/* Botón imprimir matrices */
void
on_b_print_matrices_clicked (GObject   *object, MaTestGUI *gui)
{
	GList      *iter;
	GString    *buffer;
	gchar      *straux;

	buffer = g_string_new (NULL);

	for (iter = gui->work->logic->ucons; iter; iter = iter->next) {
		straux = (gchar*) sprint_ucon_matrix_hex ((LlUCon*) iter->data, gui->work->logic);
		g_string_append (buffer, straux);
		g_free (straux);
		g_string_append_c (buffer, '\n');
	}

	for (iter = gui->work->logic->bcons; iter; iter = iter->next)	{
		straux = (gchar*) sprint_bcon_matrix_hex ((LlBCon*) iter->data, gui->work->logic);
		g_string_append (buffer, straux);
		g_free (straux);
		g_string_append_c (buffer, '\n');
	}

	/* Quitamos los dos últimos saltos de línea */
	g_string_truncate (buffer, strlen (buffer->str) - 2);

	gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gui->textbuffer), buffer->str, -1);
	g_string_free (buffer, TRUE);
}


/* Pulsar Enter en la entrada de la fórmula o pulsar el botón correspondiente */
void
on_entry_formula_activate (GObject   *object, MaTestGUI *gui)
{
	gchar    *formula,
	         *straux;
	int       i;

	formula = (gchar*) gtk_entry_get_text (GTK_ENTRY (gui->entry_formula));
	if (strcmp (formula, "")) {
		ll_wff_parse_formula (&gui->work->wff, formula, gui->work->logic);
	}
	if (gui->work->wff) {
		straux = ll_wff_sprint_formula (gui->work->wff, gui->work->logic);
		/* Quitamos los paréntesis exteriores */
		if (straux[0] == '(') {
			for (i = 0; i < strlen (straux); i++) {
				straux[i] = straux[i + 1];
			}
			if (straux[strlen (straux) - 1] == ')') {
				straux[strlen (straux) - 1] = '\0';
			}
		}

		strcpy (gui->work->formula, straux);
		g_free (straux);
		straux = g_strdup_printf ("Fórmula: %s", gui->work->formula);
		gtk_label_set_text (GTK_LABEL (gui->label_formula), straux);
		g_free (straux);
		gtk_entry_set_text (GTK_ENTRY (gui->entry_formula), "");
	}
/*
	formula = (gchar*) gtk_entry_get_text (GTK_ENTRY (gui->entry_formula));
	if (strcmp (formula, "")
	    && ll_formula_is_wff_pn (formula, gui->work->logic)) {
		strcpy (gui->work->formula_pn, formula);

		ll_logic_add_formula_vars (gui->work->logic, gui->work->formula_pn);

		if (gui->work->wff) {
			ll_wff_free (gui->work->wff);
			gui->work->wff = NULL;
		}
		gui->work->wff = ll_wff_parse_formula_pn (gui->work->wff,
		                                          gui->work->formula_pn,
		                                          gui->work->logic);
		straux = g_strdup_printf ("Fórmula: %s", gui->work->formula_pn);
		gtk_label_set_text (GTK_LABEL (gui->label_formula), straux);
		g_free (straux);
		gtk_entry_set_text (GTK_ENTRY (gui->entry_formula), "");
	}
	*/
}

/* FIXME Estas dos funciones se pueden reducir a una sola */
/* Botón de una conectiva unaria cualquiera */
void
on_b_ucon_clicked (GObject   *object, MaTestGUI *gui)
{
	gchar    *label;

	label = (gchar*) gtk_button_get_label (GTK_BUTTON (object));
	dialog_ucon_edit (gui, label);
}


/* Botón de una conectiva binaria cualquiera */
void
on_b_bcon_clicked (GObject   *object, MaTestGUI *gui)
{
	gchar    *label;

	label = (gchar*) gtk_button_get_label (GTK_BUTTON (object));
	dialog_bcon_edit (gui, label);
}


/* Botón añadir conectiva unaria */
void
on_b_add_ucon_clicked (GObject   *object, MaTestGUI *gui)
{
	dialog_ucon_new (gui);
}


void
on_b_ucon_del_clicked (GObject   *object, MaTestGUI *gui)
{

}


/* Botón añadir conectiva binaria */
void
on_b_add_bcon_clicked (GObject   *object, MaTestGUI *gui)
{
	dialog_bcon_new (gui);
}


/* Cambiar el valor de la dimensión */
void
on_spin_dimension_value_changed (GObject   *object, MaTestGUI *gui)
{
	GtkWidget    *b_con;
	GList        *iter;
	gchar        *label;

	/* Quitamos las conectivas y sus botones */
	ll_ucon_list_free (gui->work->logic->ucons);
	gui->work->logic->ucons = NULL;
	ll_bcon_list_free (gui->work->logic->bcons, gui->work->DIM);
	gui->work->logic->bcons = NULL;
	for (iter = gtk_container_get_children (GTK_CONTAINER (gui->hb_ucons));
	     iter;
	     iter = iter->next) {
		gtk_widget_destroy (GTK_WIDGET (iter->data));
	}
	for (iter = gtk_container_get_children (GTK_CONTAINER (gui->hb_bcons));
	     iter;
	     iter = iter->next) {
		gtk_widget_destroy (GTK_WIDGET (iter->data));
	}

	gui->work->DIM = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (gui->spin_dimension));

	ll_logic_set_default_ucons_lukasiewicz (gui->work->logic);
	ll_logic_set_default_bcons_lukasiewicz (gui->work->logic);

	gtk_spin_button_set_range (GTK_SPIN_BUTTON (gui->spin_mdv), 1, gui->work->MAXV);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (gui->spin_mdv), gui->work->MAXV);

	/* crea los botones para las conectivas por defecto */
	for (iter = gui->work->logic->ucons; iter; iter = iter->next)	{
		label = g_strdup_printf ("%s", ((LlUCon*) iter->data)->symbol);
		b_con = gtk_button_new_with_label (label);
		g_free (label);
		g_signal_connect (G_OBJECT (b_con), "clicked",
	                    G_CALLBACK (on_b_ucon_clicked), (gpointer) gui);
	  gtk_container_add (GTK_CONTAINER (gui->hb_ucons), b_con);
		gtk_widget_show (b_con);
	}
	for (iter = gui->work->logic->bcons; iter; iter = iter->next)	{
		label = g_strdup_printf ("%s", ((LlUCon*) iter->data)->symbol);
		b_con = gtk_button_new_with_label (label);
		g_free (label);
		g_signal_connect (G_OBJECT (b_con), "clicked",
		                  G_CALLBACK (on_b_bcon_clicked), gui);
		gtk_container_add (GTK_CONTAINER (gui->hb_bcons), b_con);
		gtk_widget_show (b_con);
	}
	
	/* Limpiamos la salida de texto */
	gtk_text_buffer_set_text (gui->textbuffer, "", 0);
}


/* Cambiar el valor del mínimo valor designado */
void
on_spin_mdv_value_changed (GObject   *object, MaTestGUI *gui)
{
	gui->work->logic->mdv = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (gui->spin_mdv));
	/* Limpiamos la salida de texto */
	gtk_text_buffer_set_text (gui->textbuffer, "", 0);
}


/* Menú > Archivo > Abrir : abre un archivo de matrices */
void
on_m_file_open_activate (GObject   *object, MaTestGUI *gui)
{
	GtkWidget   *chooser,
	            *b_con;
	GList       *iter;
	gchar       *filename = NULL,
	            *label;

	chooser = gtk_file_chooser_dialog_new ("Guardar matrices",
	                                       GTK_WINDOW (gui->window),
	                                       GTK_FILE_CHOOSER_ACTION_OPEN,
	                                       GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
	                                       GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
	                                       NULL);
	gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (chooser), TRUE);
	if (gtk_dialog_run (GTK_DIALOG (chooser)) == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
		if (filename) {
			read_matricesfile (gui->work, filename);
		}
		g_free (filename);
		
		if (gui->work->logic_modified) {
			/* Quitamos los botones de las conectivas anteriores */
			for (iter = gtk_container_get_children (GTK_CONTAINER (gui->hb_ucons));
			     iter;
			     iter = iter->next) {
				gtk_widget_destroy (GTK_WIDGET (iter->data));
			}
			for (iter = gtk_container_get_children (GTK_CONTAINER (gui->hb_bcons));
			     iter;
			     iter = iter->next) {
				gtk_widget_destroy (GTK_WIDGET (iter->data));
			}
			/* Creamos los nuevos botones */
			for (iter = gui->work->logic->ucons; iter; iter = iter->next)	{
				label = g_strdup_printf ("%s", ((LlUCon*) iter->data)->symbol);
				b_con = gtk_button_new_with_label (label);
				g_free (label);
				g_signal_connect (G_OBJECT (b_con), "clicked",
				                  G_CALLBACK (on_b_ucon_clicked), (gpointer) gui);
				gtk_container_add (GTK_CONTAINER (gui->hb_ucons), b_con);
				gtk_widget_show (b_con);
			}
			for (iter = gui->work->logic->bcons; iter; iter = iter->next)	{
				label = g_strdup_printf ("%s", ((LlUCon*) iter->data)->symbol);
				b_con = gtk_button_new_with_label (label);
				g_free (label);
				g_signal_connect (G_OBJECT (b_con), "clicked",
		                      G_CALLBACK (on_b_bcon_clicked), gui);
				gtk_container_add (GTK_CONTAINER (gui->hb_bcons), b_con);
				gtk_widget_show (b_con);
			}
			/* Ajustamos la dimensión y el MVD en la interfaz sin emitir las señales */
			g_signal_handlers_block_by_func (gui->spin_dimension, 
			                                 on_spin_dimension_value_changed, gui);
			g_signal_handlers_block_by_func (gui->spin_mdv, 
			                                 on_spin_mdv_value_changed, gui);
			gtk_spin_button_set_value (GTK_SPIN_BUTTON (gui->spin_dimension), gui->work->DIM);
			gtk_spin_button_set_range (GTK_SPIN_BUTTON (gui->spin_mdv),
			                           1, gui->work->MAXV);
			gtk_spin_button_set_value (GTK_SPIN_BUTTON (gui->spin_mdv), gui->work->MDV);
			g_signal_handlers_unblock_by_func (gui->spin_dimension, 
			                                   on_spin_dimension_value_changed, gui);
			g_signal_handlers_unblock_by_func (gui->spin_mdv, 
			                                   on_spin_mdv_value_changed, gui);
			/* Limpiamos la salida de texto */
			gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gui->textbuffer), "", 0);
			
			gui->work->logic_modified = FALSE;
		}
	}
	gtk_widget_destroy (chooser);
}


/* Menú > Archivo > Guardar : guarda las matrices en un archivo */
void
on_m_file_save_activate (GObject   *object, MaTestGUI *gui)
{
	write_matrices_gui (gui);
}


/* Menú > Ver > Todos los valores : muestra todos los valores en la evaluación */
void
on_m_view_all_toggled (GObject   *object, MaTestGUI *gui)
{
	gchar    *buffer;

	gui->work->evaluation_style = ALL;

	/* Y mostramos la evalución con el nuevo estilo */
	buffer = (gchar*) sprint_evaluation (gui->work);
	/* Quitamos el último salto de línea */
	buffer[strlen (buffer) - 1] = '\0';
	gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gui->textbuffer), buffer, -1);
	g_free (buffer);
}


/* Menú > Ver > Valores designados : muestra los valores designados en la evaluación */
void
on_m_view_desig_toggled (GObject   *object, MaTestGUI *gui)
{
	gchar    *buffer;

	gui->work->evaluation_style = DESIGNATED;

	/* Y mostramos la evalución con el nuevo estilo */
	buffer = (gchar*) sprint_evaluation (gui->work);
	/* Quitamos el último salto de línea */
	buffer[strlen (buffer) - 1] = '\0';
	gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gui->textbuffer), buffer, -1);
	g_free (buffer);
}


/* Menú > Ver > Valores no designados : muestra los valores no designados en la evaluación */
void
on_m_view_notdesig_toggled (GObject   *object, MaTestGUI *gui)
{
	gchar    *buffer;

	gui->work->evaluation_style = NOT_DESIGNATED;

	/* Y mostramos la evalución con el nuevo estilo */
	buffer = (gchar*) sprint_evaluation (gui->work);
	/* Quitamos el último salto de línea */
	buffer[strlen (buffer) - 1] = '\0';
	gtk_text_buffer_set_text (GTK_TEXT_BUFFER (gui->textbuffer), buffer, -1);
	g_free (buffer);
}


/*
 * Menú > Ayuda > Acerca de : muestra el diálogo con los créditos.
 */
void
on_m_help_about_activate (GObject   *object, gpointer data)
{
	dialog_about ();
}
