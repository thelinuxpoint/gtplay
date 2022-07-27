#include "ui.hpp"
#include "window.hpp"
#include <iostream>
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
  	m_refBuilder = Gtk::Builder::create();

	Glib::ustring ui_info =
	    "<interface>"
	    "  <!-- menubar -->"
	    "  <menu id='hy-menu'>"
	    "    <submenu>"
	    "      <attribute name='label' translatable='yes'>_Media</attribute>"
	    "      <section>"
	    "        <item>"
	    "          <attribute name='label' translatable='yes'>Open _File</attribute>"
	    // "          <attribute name='action'>win.openfile</attribute>"
	    "          <attribute name='accel'>&lt;Primary&gt;o</attribute>"
	    "        </item>"
	    "        <item>"
	    "          <attribute name='label' translatable='yes'>Open _Folder</attribute>"
	    // "          <attribute name='action'>win.openfolder</attribute>"
	    "        </item>"
	    "        <item>"
	    "          <attribute name='label' translatable='yes'>_Stream</attribute>"
	    // "          <attribute name='action'>app.newwindow</attribute>"
	    "        </item>"
	    "      </section>"
	    "      <section>"
	    "        <item>"
	    "          <attribute name='label' translatable='yes'>_Quit</attribute>"
	    // "          <attribute name='action'>app.quit</attribute>"
	    "          <attribute name='accel'>&lt;Primary&gt;q</attribute>"
	    "        </item>"
	    "      </section>"
	    "    </submenu>"
	    "    <submenu>"
	    "      <attribute name='label' translatable='yes'>_Playback</attribute>"
	    "      <section>"
	    "        <item>"
	    "          <attribute name='label' translatable='yes'>_Copy</attribute>"
	    // "          <attribute name='action'>win.copy</attribute>"
	    "          <attribute name='accel'>&lt;Primary&gt;c</attribute>"
	    "        </item>"
	    "        <item>"
	    "          <attribute name='label' translatable='yes'>_Paste</attribute>"
	    // "          <attribute name='action'>win.paste</attribute>"
	    "          <attribute name='accel'>&lt;Primary&gt;v</attribute>"
	    "        </item>"
	    "      </section>"
	    "    </submenu>"
	    "    <submenu>"
	    "      <attribute name='label' translatable='yes'>_Audio</attribute>"
	    "      <section>"
	    "        <item>"
	    "          <attribute name='label' translatable='yes'>_Title</attribute>"
	    // "          <attribute name='action'>win.copy</attribute>"
	    "          <attribute name='accel'>&lt;Primary&gt;c</attribute>"
	    "        </item>"
	    "        <item>"
	    "          <attribute name='label' translatable='yes'>_Paste</attribute>"
	    // "          <attribute name='action'>win.paste</attribute>"
	    "          <attribute name='accel'>&lt;Primary&gt;v</attribute>"
	    "        </item>"
	    "      </section>"
	    "    </submenu>"
	    "    <submenu>"
	    "      <attribute name='label' translatable='yes'>_Help</attribute>"
	    "      <section>"
	    "        <item>"
	    "          <attribute name='label' translatable='yes'>_about</attribute>"
	    // "          <attribute name='action'>win.copy</attribute>"
	    "          <attribute name='accel'>&lt;Primary&gt;c</attribute>"
	    "        </item>"
	    "      </section>"
	    "    </submenu>"
	    "  </menu>"
	    "  <!-- application menu -->"
	    "  <menu id='appmenu'>"
	    "  </menu>"
	    "</interface>";
	try{
    	m_refBuilder->add_from_string(ui_info);
  	}
  	catch (const Glib::Error& ex){
    	std::cerr << "Building menus failed: " << ex.what();
  	}

  	auto object = m_refBuilder->get_object("hy-menu");
  	auto gmenu = Glib::RefPtr<Gio::Menu>::cast_dynamic(object);

  	if (!gmenu) {
    	g_warning("GMenu or AppMenu not found");
  	}
  	else{
	    set_menubar(gmenu);
  	}

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

	add_window(*win);

	win->signal_hide().connect(sigc::bind<Gtk::Window*>(sigc::mem_fun(*this, &GTP::GPlayApp::on_window_hide), win));
  	win->show_all();
}
/* ###########################
	
*/
void GTP::GPlayApp::on_new_window(){

	auto win = new GTP::GWindow();

	add_window(*win);
	win->signal_hide().connect(sigc::bind<Gtk::Window*>(sigc::mem_fun(*this, &GTP::GPlayApp::on_window_hide), win));
  	win->show_all();

}
/* ###########################
	
*/
void GTP::GPlayApp::on_window_hide(Gtk::Window* window){
  	delete window;
}
