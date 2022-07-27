#include "gcpanel.hpp"

GTP::GPanel::GPanel(): Gtk::VBox(){
	gb_play = Gtk::manage(new Gtk::Button("play"));
	pack_end(*gb_play,false,false,5);
	show_all();
}
