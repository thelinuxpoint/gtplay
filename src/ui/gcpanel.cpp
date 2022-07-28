#include "gcpanel.hpp"

GTP::GPanel::GPanel(): Gtk::VBox(){
	Gdk::RGBA black_backk;
    black_backk.set("#121417");

    this->override_background_color(black_backk);

	hbox = Gtk::manage(new Gtk::HBox());
    // #########################
	gb_play = Gtk::manage(new Gtk::Button());
	gb_play->set_size_request(10,10);
    gb_play->set_image_from_icon_name("media-playback-start");
    // #########################
	gb_previous = Gtk::manage(new Gtk::Button());
	gb_previous->set_size_request(10,10);
    gb_previous->set_image_from_icon_name("media-skip-backward");
    // #########################
	gb_forward = Gtk::manage(new Gtk::Button());
	gb_forward->set_size_request(10,10);
    gb_forward->set_image_from_icon_name("media-seek-forward");

    g_pbr = Gtk::manage(new Gtk::Scale());

    vol = Gtk::manage(new Gtk::ScaleButton(Gtk::IconSize(2),0,1,1));
	vol->set_image_from_icon_name("audio-volume-low");
	hbox->pack_start(*gb_previous,false,false,5);
	hbox->pack_start(*gb_play,false,false,5);
	hbox->pack_start(*gb_forward,false,false,5);
	hbox->pack_start(*g_pbr);
	g_pbr->set_draw_value(false);
	hbox->pack_end(*vol,false,false,5);

	pack_end(*hbox,false,false,10);
	show_all();
}
