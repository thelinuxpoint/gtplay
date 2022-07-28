#include <gtkmm.h>


namespace GTP{
	
	class GPanel : public Gtk::VBox {
		
		public:
			GPanel();
			Gtk::Button *gb_play;
			Gtk::Button *gb_forward;
			Gtk::Button *gb_previous;
			Gtk::HBox *hbox;
			Gtk::ScaleButton *vol;

			Gtk::Scale *g_pbr;
	};
}

