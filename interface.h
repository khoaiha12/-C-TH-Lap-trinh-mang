
static gboolean get_x_loc (GtkWidget *widget, GdkEvent *event, gpointer data);

static gboolean get_y_loc (GtkWidget *widget, GdkEvent *event, gpointer data);

static gboolean make_move (GtkWidget *widget, GdkEvent *event, gpointer data);

void set_message (GtkWidget *w,gpointer data);

void init_play_window();

void init_home_window ();

void add_to_list(GtkWidget *tlist, const gchar *str);

void append_item(GtkWidget *widget, gpointer entry);

void remove_item(GtkWidget *widget, gpointer selection);

void remove_all(GtkWidget *widget, gpointer selection);

void init_list(GtkWidget *tlist);

void on_play_button_clicked();

void on_back_button_clicked();

void on_newgame_button_clicked();

void on_exit_button_clicked();

void close_window_main();
