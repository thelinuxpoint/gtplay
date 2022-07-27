#include "window.hpp"
#include "gcpanel.hpp"
#include <iostream>

GTP::GWindow::GWindow(){
	set_title("GTPlay ~ v0.1.0");
    set_default_size(600, 400);
    GTP::GPanel v;
    add(v);

}
GTP::GWindow::~GWindow(){
	
	
}
