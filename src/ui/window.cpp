#include "window.hpp"
#include <iostream>

GTP::GWindow::GWindow(){
	set_title("GTPlay ~ v0.1.0");
    set_default_size(600, 400);
    mainbox = Gtk::manage(new Gtk::VBox());
    mainbox2 = Gtk::manage(new Gtk::Box());
    pbox = Gtk::manage(new GTP::GPanel());
	pbox->set_size_request(-1,20);
    mainbox->pack_start(*mainbox2,true,true,0);
    mainbox->pack_end(*pbox,Gtk::PACK_SHRINK,0);


    add(*mainbox);

}
GTP::GWindow::~GWindow(){
	
}
