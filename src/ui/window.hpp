#include <gtkmm.h>
#include "gcpanel.hpp"

namespace GTP {
	class GWindow : public Gtk::ApplicationWindow {
  		
  		public:	
  			GWindow();
  			GTP::GPanel *pbox;
  			Gtk::VBox *mainbox;
  			Gtk::Box *mainbox2;
  			

  			virtual ~GWindow();
	};
}
