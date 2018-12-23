#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <gtk/gtk.h>
#include "close.h"
#include "testwin.h"
#include "interface.h"
#include "client_params.h"

int iLocation [2];
char cBoardLoc[10][10] = { {'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E'}, {'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E'},
						   {'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E'}, {'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E'},
						   {'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E'}, {'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E'},
						   {'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E'}, {'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E'},
						   {'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E'}, {'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E'} };
char cTurn = 'X';

enum {
    
	LIST_ITEM = 0,
	N_COLUMNS
};

GtkWidget *list;
GtkWidget *window_main;
GtkWidget *window_home;
GtkWidget *fixed_main;
GtkWidget *fixed_home;
GtkWidget *view;
GtkWidget *entry_mes;

static gboolean get_x_loc (GtkWidget *widget, GdkEvent *event, gpointer data)
{
	iLocation [0] = (int) data;
	
	return FALSE;
}

static gboolean get_y_loc (GtkWidget *widget, GdkEvent *event, gpointer data)
{
	iLocation [1] = (int) data;
	
	return FALSE;
}

static gboolean make_move (GtkWidget *widget, GdkEvent *event, gpointer data)
{
	int bWon;
	int x = 0, y = 0;
	int iXPos = 0;
	int iYPos = 0;
	GtkWidget *player;
	GtkWidget *dialog_win, *dialog_invalid;
	GtkResponseType result;
	char cImgLoc [16] = "./images/";
	strcat (cImgLoc, &cTurn);
	strcat (cImgLoc, ".png");
	
	if (cBoardLoc [iLocation [0]][iLocation [1]] == 'E')
	{
		cBoardLoc [iLocation [0]][iLocation [1]] = cTurn;
		// thực hiện gửi nhận server chỗ này
		while (x < iLocation [0])
		{	
			iXPos += 60;
			x++;
		}
			
		while (y < iLocation [1])
		{	
			iYPos += 60;
			y++;
		}
			
		player = gtk_image_new_from_file (cImgLoc);
		gtk_fixed_put (GTK_FIXED (fixed_main), player, iXPos, iYPos);
		gtk_widget_show (player);
		
		bWon = checkWin(iLocation [0],iLocation [1],cBoardLoc,cTurn);
		if (bWon == 1)
		{
			printf ("%c Won the game\n", cTurn);
			
			dialog_win = gtk_message_dialog_new (NULL,GTK_DIALOG_MODAL,GTK_MESSAGE_INFO,GTK_BUTTONS_CLOSE,
												"%c has won the game\n", cTurn);
																	
			result = gtk_dialog_run (GTK_DIALOG (dialog_win));
			gtk_widget_destroy (dialog_win);

		}
		else
		{		
			if (cTurn == 'X')
				cTurn = 'O';
			else
				cTurn = 'X';
		}
		if(result == GTK_RESPONSE_CLOSE){
			on_newgame_button_clicked();
		}

	}
	else
	{
		g_print ("Invalid move!\n");
			dialog_invalid = gtk_message_dialog_new (NULL,GTK_DIALOG_MODAL,GTK_MESSAGE_ERROR,GTK_BUTTONS_CLOSE,
												"%c has mad an invalid move, try again!", cTurn);
												
			gtk_dialog_run (GTK_DIALOG (dialog_invalid));
			gtk_widget_destroy (dialog_invalid);
	}
		
	return TRUE;
}
void set_message (GtkWidget *w,gpointer data) 
{
	gchar *entry_text, *view_text;
	char text[1000];

	entry_text = gtk_entry_get_text(GTK_ENTRY(entry_mes));
	//g_print("%s\n",entry_text);

	GtkTextBuffer *buffer;
    GtkTextIter start, end,iter;

    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(GTK_TEXT_VIEW(view)));
    gtk_text_buffer_get_iter_at_offset(buffer, &iter, 0);

    gtk_text_buffer_insert(buffer, &iter, "", -1);
    gtk_text_buffer_get_bounds (buffer, &start, &end);
    view_text = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);

	if(strcmp(view_text,"")!=0)
	{
		strcpy(text, view_text);
		strcat(text, "\n");
		strcat(text, entry_text);
	}
	else
	{
		strcpy(text,entry_text);		
	}

	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
	if (buffer == NULL)
		buffer = gtk_text_buffer_new(NULL);
	gtk_text_buffer_set_text(buffer, text, -1);
	gtk_text_view_set_buffer(GTK_TEXT_VIEW(view), buffer);
	gtk_entry_set_text(GTK_ENTRY(entry_mes),"");

}

void init_play_window() 
{
	int x, y;
	int iXPos = 0;
	int iYPos = 0;

	GtkWidget *image_board;
	GtkWidget *eventbox_main;
	GtkWidget *scrolling;
	GtkWidget *send_button, *newgame_button, *back_button;

	window_main = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	g_signal_connect (G_OBJECT (window_main), "delete_event", G_CALLBACK (delete_event), NULL);
	g_signal_connect (G_OBJECT (window_main), "destroy", G_CALLBACK (destroy), NULL);
	gtk_window_set_title (GTK_WINDOW (window_main), "Caro made by Tạ team");
	gtk_window_set_position(GTK_WINDOW(window_main), GTK_WIN_POS_CENTER);
	gtk_container_set_border_width (GTK_CONTAINER (window_main), 15);

	
	image_board = gtk_image_new_from_file ("./images/caro.png");
	
	fixed_main = gtk_fixed_new ();
	gtk_container_add (GTK_CONTAINER (window_main), fixed_main);
	gtk_fixed_put (GTK_FIXED (fixed_main), image_board, 0, 0);

	view = gtk_text_view_new();
  	gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW(view), GTK_WRAP_WORD);
	gtk_text_view_set_editable (GTK_TEXT_VIEW(view), FALSE);
	gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW(view), FALSE);
	gtk_text_view_set_pixels_above_lines (GTK_TEXT_VIEW(view), 3);
	gtk_text_view_set_pixels_below_lines (GTK_TEXT_VIEW(view), 3);

	scrolling = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scrolling), view);
	gtk_widget_set_size_request(scrolling, 300, 350);
	//gtk_scrolled_window_set_placement (GTK_SCROLLED_WINDOW (scrolling), GTK_CORNER_BOTTOM_LEFT);
	//gtk_scrolled_window_set_max_content_width (GTK_SCROLLED_WINDOW (scrolling), 50);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolling),GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_fixed_put (GTK_FIXED (fixed_main), scrolling, 620, 10);
	

	entry_mes = gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (entry_mes), 150);
	gtk_entry_set_placeholder_text (GTK_ENTRY (entry_mes), "Type your message ...");
	gtk_widget_set_size_request(entry_mes, 250, 35);
	gtk_fixed_put (GTK_FIXED (fixed_main), entry_mes, 620, 375);
	
	send_button = gtk_button_new_with_label ("Send");
	gtk_widget_set_size_request(send_button, 40, 35);
	gtk_fixed_put (GTK_FIXED (fixed_main), send_button, 880, 375);

	newgame_button = gtk_button_new_with_label ("New game");
	gtk_widget_set_size_request(newgame_button, 100, 50);
	gtk_fixed_put (GTK_FIXED (fixed_main), newgame_button, 650, 475);

	back_button = gtk_button_new_with_label ("Back");
	gtk_widget_set_size_request(back_button, 100, 50);
	gtk_fixed_put (GTK_FIXED (fixed_main), back_button, 780, 475);

	g_signal_connect(G_OBJECT(newgame_button), "clicked", G_CALLBACK(on_newgame_button_clicked), NULL);

	g_signal_connect(G_OBJECT(back_button), "clicked", G_CALLBACK(on_back_button_clicked), NULL);

	g_signal_connect(G_OBJECT(entry_mes), "activate", G_CALLBACK(set_message), NULL);

	g_signal_connect(G_OBJECT(send_button), "clicked", G_CALLBACK(set_message),NULL);


	for (y = 0; y <= 9; y++)
	{
		for (x = 0; x <= 9; x++)
		{
			eventbox_main = gtk_event_box_new ();
			g_signal_connect (G_OBJECT (eventbox_main), "button_press_event", G_CALLBACK (get_x_loc), (gpointer) x);
			g_signal_connect (G_OBJECT (eventbox_main), "button_press_event", G_CALLBACK (get_y_loc), (gpointer) y);
			g_signal_connect (G_OBJECT (eventbox_main), "button_press_event", G_CALLBACK (make_move), NULL);
			gtk_widget_set_size_request (eventbox_main, 60, 60);
			gtk_fixed_put (GTK_FIXED (fixed_main), eventbox_main, iXPos, iYPos);
			gtk_event_box_set_visible_window (GTK_EVENT_BOX (eventbox_main), FALSE);			
			gtk_widget_show (eventbox_main);
					
			iXPos +=60;
		}
		iYPos += 60;
		iXPos = 0;
	}
	
	gtk_widget_show (image_board);
	gtk_widget_show (fixed_main);
	gtk_widget_show (view);
	gtk_widget_show (scrolling);
	gtk_widget_show (send_button);
	gtk_widget_show (newgame_button);
	gtk_widget_show (back_button);
	gtk_widget_show (entry_mes);
	gtk_widget_show (window_main);
	
	gtk_main();

	return;
}

void init_home_window ()
{
	GtkWidget *image;
	GtkWidget *entry_name;
	GtkWidget *play_button, *exit_button, *set_button;
	GtkWidget *scrolling;
	GtkWidget *vbox;
	GtkWidget *label;
	GtkTreeSelection *selection;

	window_home = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	g_signal_connect (G_OBJECT (window_home), "delete_event", G_CALLBACK (delete_event), NULL);
	g_signal_connect (G_OBJECT (window_home), "destroy", G_CALLBACK (destroy), NULL);
	gtk_window_set_title (GTK_WINDOW (window_home), "Caro made by Tạ team");
	gtk_window_set_default_size(GTK_WINDOW(window_home), 1000, 600);
	gtk_window_set_position(GTK_WINDOW(window_home), GTK_WIN_POS_CENTER);
	//gtk_container_set_border_width (GTK_CONTAINER (window_home), 15);

	fixed_home = gtk_fixed_new ();
	gtk_container_add (GTK_CONTAINER (window_home), fixed_home);

	image = gtk_image_new_from_file("./images/background.png");
    gtk_container_add(GTK_CONTAINER(fixed_home), image);

	entry_name = gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (entry_name), 150);
	gtk_entry_set_placeholder_text (GTK_ENTRY (entry_name), "Type your nickname ...");
	gtk_widget_set_size_request(entry_name, 250, 40);
	gtk_fixed_put (GTK_FIXED (fixed_home), entry_name, 190, 215);

	set_button = gtk_button_new_with_label ("Set");
	gtk_widget_set_size_request(set_button, 60, 41);
	gtk_fixed_put (GTK_FIXED (fixed_home), set_button, 480, 215);

	list = gtk_tree_view_new();
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(list), FALSE);

  	vbox = gtk_vbox_new(FALSE, 0);

  	gtk_box_pack_start(GTK_BOX(vbox), list, TRUE, TRUE, 5);

  	label = gtk_label_new("");
  	gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 5);

  	//gtk_container_add(GTK_CONTAINER(fixed), vbox);

	scrolling = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scrolling), vbox);
	gtk_widget_set_size_request(scrolling, 180, 220);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolling),GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_fixed_put (GTK_FIXED (fixed_home), scrolling, 760, 80);


  	init_list(list);
  	add_to_list(list, "Bzozo");
  	add_to_list(list, "Cún");
  	add_to_list(list, "Uchiha");
  	add_to_list(list, "hihi");

  	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(list));

	play_button = gtk_button_new_with_label ("Invite");
	gtk_widget_set_size_request(play_button, 100, 50);
	gtk_fixed_put (GTK_FIXED (fixed_home), play_button, 800, 300);

	exit_button = gtk_button_new_with_label ("Exit");
	gtk_widget_set_size_request(exit_button, 80, 40);
	gtk_fixed_put (GTK_FIXED (fixed_home), exit_button, 30, 530);

	g_signal_connect (G_OBJECT(play_button), "clicked", G_CALLBACK(on_play_button_clicked), NULL);

	g_signal_connect (G_OBJECT(exit_button), "clicked", G_CALLBACK(on_exit_button_clicked), NULL);

	gtk_widget_show (fixed_home);
	gtk_widget_show (image);
	gtk_widget_show (entry_name);
	gtk_widget_show (set_button);
	gtk_widget_show (list);
	gtk_widget_show (vbox);
	gtk_widget_show (scrolling);
	gtk_widget_show (label);
	gtk_widget_show (play_button);
	gtk_widget_show (exit_button);
	gtk_widget_show (window_home);
	
	gtk_main();
	return;
	
}

void add_to_list(GtkWidget *tlist, const gchar *str) {
    
  GtkListStore *store;
  GtkTreeIter iter;

  store = GTK_LIST_STORE(gtk_tree_view_get_model
      (GTK_TREE_VIEW(tlist)));

  gtk_list_store_append(store, &iter);
  gtk_list_store_set(store, &iter, LIST_ITEM, str, -1);
}

void append_item(GtkWidget *widget, gpointer entry) {
    
  GtkListStore *store;
  GtkTreeIter iter;

  const gchar *str = gtk_entry_get_text(entry); 

  store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(list)));

  gtk_list_store_append(store, &iter);
  gtk_list_store_set(store, &iter, LIST_ITEM, str, -1);
  
}

void remove_item(GtkWidget *widget, gpointer selection) {
    
  GtkListStore *store;
  GtkTreeModel *model;
  GtkTreeIter  iter;

  store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(list)));
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(list));

  if (gtk_tree_model_get_iter_first(model, &iter) == FALSE) {
      return;
  }

  if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(selection), 
         &model, &iter)) {
    gtk_list_store_remove(store, &iter);
  }
}

void remove_all(GtkWidget *widget, gpointer selection) {
    
  GtkListStore *store;
  GtkTreeModel *model;
  GtkTreeIter  iter;

  store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(list)));
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(list));

  if (gtk_tree_model_get_iter_first(model, &iter) == FALSE) {
      return;
  }
  
  gtk_list_store_clear(store);
}


void init_list(GtkWidget *tlist) {

  GtkCellRenderer    *renderer;
  GtkTreeViewColumn  *column;
  GtkListStore       *store;

  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes("List Item",renderer, "text", LIST_ITEM, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(tlist), column);

  store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING);

  gtk_tree_view_set_model(GTK_TREE_VIEW(tlist), GTK_TREE_MODEL(store));

  g_object_unref(store);
}

void on_play_button_clicked()
{
	gtk_widget_hide(window_home);
	int x,y;
	for (y = 0; y <= 9; y++)
	{
		for (x = 0; x <= 9; x++)
		{
			cBoardLoc [x][y] = 'E';
		}
	}
	init_play_window();
}

void on_back_button_clicked()
{
	gtk_widget_hide(window_main);
	gtk_widget_show(window_home);
}

void on_newgame_button_clicked()
{
	gtk_widget_hide(window_main);
	int x,y;
	for (y = 0; y <= 9; y++)
	{
		for (x = 0; x <= 9; x++)
		{
			cBoardLoc [x][y] = 'E';
		}
	}
	init_play_window();
}

void on_exit_button_clicked()
{
	gtk_widget_destroy(GTK_WIDGET(window_home));
	gtk_main_quit();
	exit(0);
}