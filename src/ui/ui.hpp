#include <gtkmm.h>


namespace GTP{
	
	class GPlayApp :public Gtk::Application {

		private:
			void create_window();
  			void on_window_hide(Gtk::Window* window);
  			void on_new_window();

		protected:
			GPlayApp();
  			
  			void on_startup() override;
  			void on_activate() override;

		public:	

  			Glib::RefPtr<Gtk::Builder> m_refBuilder;
 			static Glib::RefPtr<GPlayApp> create();

	};


}
