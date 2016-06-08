#include <gtk/gtk.h>
#include "pocketsphinx_gtk.h"
#include <glib/gprintf.h>

pthread_t mic_thread;
GtkWidget *button_start;
GdkScreen *screen;
GtkCssProvider *provider;
GdkDisplay *display;


void change_btncolor(const gchar *color){
    provider = gtk_css_provider_new ();
    gtk_style_context_add_provider_for_screen (screen,
                                               GTK_STYLE_PROVIDER (provider),
                                               GTK_STYLE_PROVIDER_PRIORITY_USER);
    gchar css[300];
    g_sprintf(css,  " GtkWindow {\n"
            "   background-color: %s;\n"
            "}\n"
            " #mybutton {\n"
            "   -GtkWidget-focus-line-width: 0;\n"
            "   border-radius: 15px;\n"
            "   font: Sans 16;\n"
            "   color: blue;\n"
            "   border-style: outset;\n"
            "   border-width: 2px;\n"
            "   padding: 2px;\n"
            "}\n" , color);
    gtk_css_provider_load_from_data (GTK_CSS_PROVIDER (provider), css, -1, NULL);
    g_object_unref (provider);
}

void ClickCallback(GtkWidget *widget, GdkEventButton *event, gpointer callback_data)
{
    // show which button was clicked
    if (change_decoder_state()){
        gdk_threads_add_idle((GSourceFunc)change_btncolor,(gpointer)"red");
        gtk_button_set_label(GTK_BUTTON(button_start), "Touch to start listening.");
    } else {
        gdk_threads_add_idle((GSourceFunc)change_btncolor,(gpointer)"yellow");
        gtk_button_set_label(GTK_BUTTON(button_start), "Listening...");
    }
}

int render_gtk(int argc, char *argv[]){
    GtkWidget *window;

    gtk_init(&argc, &argv);

    /* Create the main, top level window */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    /* Give it the title */
    gtk_window_set_title(GTK_WINDOW(window), "Vaani");

    /* Center the window */
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    /* Set the window's default size */
    gtk_window_set_default_size(GTK_WINDOW(window), 1024, 768);

    /*
    ** Map the destroy signal of the window to gtk_main_quit;
    ** When the window is about to be destroyed, we get a notification and
    ** stop the main GTK+ loop by returning 0
    */
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    /*
    ** Assign the variable "label" to a new GTK label,
    ** with the text "Hello, world!"
    */

    // Add the button onto the main window
    button_start = gtk_button_new_with_label ("Touch to start listening.");
    gtk_widget_set_name (GTK_WIDGET(button_start),
                         "mybutton");        /* name button so we can apply css to it later */

    gtk_widget_set_halign (GTK_WIDGET(button_start),
                           GTK_ALIGN_CENTER);

    gtk_widget_set_valign (GTK_WIDGET(button_start),
                           GTK_ALIGN_CENTER);

    gtk_widget_set_size_request (GTK_WIDGET(button_start),
                                 100, 75);

    gtk_container_add (GTK_CONTAINER(window),
                       button_start);

    display = gdk_display_get_default ();
    screen = gdk_display_get_default_screen (display);
    change_btncolor("red");
    gtk_widget_show_all (window);
    g_signal_connect(G_OBJECT(button_start), "button_press_event", G_CALLBACK(ClickCallback), NULL);

    // inicia thread mike
    pthread_create(&mic_thread, NULL, recognize_from_microphone, NULL);

    /* Make sure that everything, window and label, are visible */
    gtk_widget_show_all(window);

    gtk_main();
}

int main (int argc, char *argv[])
{
    pocketsphinxstart();
    render_gtk(argc, argv);
    destroy_ps();
    return 0;
}