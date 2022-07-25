#include "ui.hpp"
#include "window.hpp"

/* ###########################
	
*/
GTP::GPlayApp::GPlayApp(): Gtk::Application("org.gtkmm.example.main_menu"){
	Glib::set_application_name("GTPlay ~ v0.1.0");
}
/* ###########################
	
*/
Glib::RefPtr<GTP::GPlayApp> GTP::GPlayApp::create(){
  return Glib::RefPtr<GTP::GPlayApp>(new GTP::GPlayApp());
}
/* ###########################
	@Function - on_startup
*/
void GTP::GPlayApp::on_startup(){

  	Gtk::Application::on_startup();


}
/* ###########################
	
*/
void GTP::GPlayApp::on_activate(){

	create_window();

}
/* ###########################
	
*/
void GTP::GPlayApp::create_window(){

	auto win = new GTP::GWindow();

	win->signal_hide().connect(sigc::bind<Gtk::Window*>(sigc::mem_fun(*this, &GTP::GPlayApp::on_window_hide), win));
  	win->show_all();

	add_window(*win);
}
/* ###########################
	
*/
void GTP::GPlayApp::on_new_window(){

	auto win = new GTP::GWindow();

	win->signal_hide().connect(sigc::bind<Gtk::Window*>(sigc::mem_fun(*this, &GTP::GPlayApp::on_window_hide), win));
  	win->show_all();

	add_window(*win);
}
/* ###########################
	
*/
void GTP::GPlayApp::on_window_hide(Gtk::Window* window){
  	delete window;
}
