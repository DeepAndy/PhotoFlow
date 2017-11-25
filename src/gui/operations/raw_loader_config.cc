/* 
 */

/*

    Copyright (C) 2014 Ferrero Andrea

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.


 */

/*

    These files are distributed with PhotoFlow - http://aferrero2707.github.io/PhotoFlow/

 */

#include "raw_loader_config.hh"


PF::RawLoaderConfigGUI::RawLoaderConfigGUI( PF::Layer* layer ):
  OperationConfigGUI( layer, "Open a RAW image" ),
  openButton(Gtk::Stock::OPEN)
{
  label.set_text( "RAW file name:" );

  controlsBox.pack_start( label );
  controlsBox.pack_start( fileEntry );
  controlsBox.pack_start( openButton );
  
  add_widget( controlsBox );

  fileEntry.signal_activate().
    connect(sigc::mem_fun(*this,
			  &RawLoaderConfigGUI::on_filename_changed));
  openButton.signal_clicked().connect(sigc::mem_fun(*this,
						    &RawLoaderConfigGUI::on_button_open_clicked) );
}


void PF::RawLoaderConfigGUI::on_filename_changed()
{
  if( get_layer() && get_layer()->get_image() && 
      get_layer()->get_processor() &&
      get_layer()->get_processor()->get_par() ) {
    std::string filename = fileEntry.get_text();
    //std::cout<<"New image file name: "<<filename<<std::endl;
    PF::RawLoaderPar* par =
      dynamic_cast<PF::RawLoaderPar*>(get_layer()->get_processor()->get_par());
    if( par && !filename.empty() ) {
      par->set_file_name( filename );
      get_layer()->set_dirty( true );
      //std::cout<<"  updating image"<<std::endl;
      get_layer()->get_image()->update();
    }
  }
}


void PF::RawLoaderConfigGUI::open()
{
  if( get_layer() && get_layer()->get_image() && 
      get_layer()->get_processor() &&
      get_layer()->get_processor()->get_par() ) {
    PF::RawLoaderPar* par =
      dynamic_cast<PF::RawLoaderPar*>(get_layer()->get_processor()->get_par());
    if( par ) {
      fileEntry.set_text( par->get_file_name() );
      //brightnessAdj.set_value( par->get_brightness() );
      //contrastAdj.set_value( par->get_contrast() );
    }
  }
  OperationConfigGUI::open();
}



void PF::RawLoaderConfigGUI::on_button_open_clicked()
{
  Gtk::FileChooserDialog dialog("Please choose a RAW file",
				Gtk::FILE_CHOOSER_ACTION_OPEN);
  //dialog.set_transient_for(*this);
  
  //Add response buttons the the dialog:
  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);

  Glib::ustring last_dir = PF::PhotoFlow::Instance().get_options().get_last_visited_image_folder();
  if( !last_dir.empty() ) dialog.set_current_folder( last_dir );

  //Show the dialog and wait for a user response:
  int result = dialog.run();

  //Handle the response:
  switch(result) {
  case(Gtk::RESPONSE_OK): 
    {
      std::cout << "Open clicked." << std::endl;

      last_dir = dialog.get_current_folder();
      PF::PhotoFlow::Instance().get_options().set_last_visited_image_folder( last_dir );

      //Notice that this is a std::string, not a Glib::ustring.
      std::string filename = dialog.get_filename();
      std::cout << "File selected: " <<  filename << std::endl;
      fileEntry.set_text( filename.c_str() );
      on_filename_changed();
      break;
    }
  case(Gtk::RESPONSE_CANCEL): 
    {
      std::cout << "Cancel clicked." << std::endl;
      break;
    }
  default: 
    {
      std::cout << "Unexpected button clicked." << std::endl;
      break;
    }
  }
}
