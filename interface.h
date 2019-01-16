
GtkWidget *list;
GtkWidget *window_main;
GtkWidget *window_home;
GtkWidget *window_choose_room;
GtkWidget *fixed_main;
GtkWidget *fixed_home;
GtkWidget *fixed_choose_room;
GtkWidget *table;
GtkWidget *eventbox_main[10][10];
GtkWidget *view;
GtkWidget *entry_mes;
GtkWidget *entry_name;
GtkWidget *set_button, *choose_room_button;
GtkWidget *label_name, *button_room[10];
GtkWidget *window_wait, *fixed_wait, *label_wait;

static gboolean get_x_loc (GtkWidget *widget, GdkEvent *event, gpointer data);

static gboolean get_y_loc (GtkWidget *widget, GdkEvent *event, gpointer data);

static gboolean make_move (GtkWidget *widget, GdkEvent *event, gpointer data);

void set_move(char *data);

void set_message (GtkWidget *w,gpointer data);

void convert_room_detail(char *data);

void wait_player_window(char *data);

void init_play_window(char *data);

void init_home_window ();

void init_choose_room_window();

void add_to_list(GtkWidget *tlist, const gchar *str);

void append_item(GtkWidget *widget, gpointer entry);

void remove_item(GtkWidget *widget, gpointer selection);

void remove_all(GtkWidget *widget, gpointer selection);

void init_list(GtkWidget *tlist);

void on_choose_room_button_clicked();

void on_room_button_clicked();

void on_back_button_clicked();

void on_newgame_button_clicked();

void on_exit_button_clicked();

void on_set_button_clicked();

void close_window_main();

void room_full_notice();
void hide_room_select();