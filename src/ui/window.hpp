#include <gtkmm.h>
#include <thread>
#include "gcpanel.hpp"

namespace GTP {
	class GWindow : public Gtk::ApplicationWindow {
  		
  		public:	
  			GWindow();
  			virtual ~GWindow();

  			GTP::GPanel *pbox;
  			Gtk::VBox *mainbox;
  			Gtk::Box *mainbox2;
			int timer=0;
			std::thread *thr;
  			// ###########################################
  			void on_open();
			
			void on_button_drag_data_get(const Glib::RefPtr<Gdk::DragContext>& context,Gtk::SelectionData& selection_data, guint info, guint time);
			
			void on_label_drop_drag_data_received(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y,const Gtk::SelectionData& selection_data, guint info, guint time);
	  		
	  		void on_drag_play(std::string);

			std::vector<unsigned char> get_file(std::string dir,bool d);
	};
}
