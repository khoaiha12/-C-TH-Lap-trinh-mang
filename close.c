#include <gtk/gtk.h>
#include "close.h"

gboolean delete_event (GtkWidget *widget, GdkEvent *event, gpointer data)
{
	
	return FALSE;
}

void destroy (GtkWidget *widget, gpointer data)
{
	gtk_main_quit ();
	exit(1);
}
void close_window(GtkWidget *widget, gpointer window)
{
    gtk_widget_destroy(GTK_WIDGET(window));
}

// gcc main.c close.c testwin.c -o run  `pkg-config --cflags --libs gtk+-3.0`