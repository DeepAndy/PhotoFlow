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

#include "slider.hh"


PF::NumEntry::NumEntry(): Gtk::Entry(), digits(1), inhibited(false)
{
  signal_activate().connect( sigc::mem_fun(*this,&PF::NumEntry::text_changed) );

  signal_focus_out_event().connect( sigc::mem_fun(*this,&PF::NumEntry::my_on_focus_out) );

  signal_key_press_event().connect( sigc::mem_fun(*this,&PF::NumEntry::on_key_press_or_release_event), false );
  signal_key_release_event().connect( sigc::mem_fun(*this,&PF::NumEntry::on_key_press_or_release_event), false );
  add_events( Gdk::KEY_PRESS_MASK | Gdk::KEY_RELEASE_MASK | Gdk::FOCUS_CHANGE_MASK );
  set_width_chars(5);
}


bool PF::NumEntry::on_key_press_or_release_event(GdkEventKey* event)
{
  if (event->type == GDK_KEY_PRESS &&
      (event->state & (GDK_SHIFT_MASK | GDK_CONTROL_MASK | GDK_MOD1_MASK)) == 0) {
    if( (event->keyval == GDK_KEY_Up) ) {
      std::cout<<"Pressed "<<event->keyval<<" key"<<std::endl;
      float new_val = adjustment->get_value();
      new_val += adjustment->get_step_increment();
      adjustment->set_value( new_val );
      return true;
    }
    if( event->keyval == GDK_KEY_Down ) {
      std::cout<<"Pressed "<<event->keyval<<" key"<<std::endl;
      float new_val = adjustment->get_value();
      new_val -= adjustment->get_step_increment();
      adjustment->set_value( new_val );
      return true;
    }
    if( event->keyval == GDK_KEY_Page_Up ) {
      std::cout<<"Pressed "<<event->keyval<<" key"<<std::endl;
      float new_val = adjustment->get_value();
      new_val += adjustment->get_page_increment();
      adjustment->set_value( new_val );
      return true;
    }
    if( event->keyval == GDK_KEY_Page_Down ) {
      std::cout<<"Pressed "<<event->keyval<<" key"<<std::endl;
      float new_val = adjustment->get_value();
      new_val -= adjustment->get_page_increment();
      adjustment->set_value( new_val );
      return true;
    }
  }
  return false;
}


#ifdef GTKMM_2
void PF::NumEntry::set_adjustment( Gtk::Adjustment* a )
#endif
#ifdef GTKMM_3
void PF::NumEntry::set_adjustment( Glib::RefPtr<Gtk::Adjustment> a )
#endif
{
  adjustment = a;
  adjustment->signal_value_changed().
      connect(sigc::mem_fun(*this,
          &NumEntry::changed));
  changed();
}


void PF::NumEntry::changed()
{
  inhibited = true;
  std::ostringstream str;
  str << adjustment->get_value();
  set_text( str.str() );
  inhibited = false;
}


void PF::NumEntry::text_changed()
{
  if( inhibited ) return;
  std::istringstream str( get_text() );
  float val;
  str >> val;
  adjustment->set_value( val );
}


void PF::Slider::create_widgets( std::string l, double val,
    double min, double max,
    double sincr, double pincr )
{
#ifdef GTKMM_2
  numentry.set_adjustment( &adjustment );
#endif
#ifdef GTKMM_3
  adjustment = Gtk::Adjustment::create( val, min, max, sincr, pincr, 0 );
  scale.set_adjustment( adjustment );
  spinButton.set_adjustment( adjustment );
  numentry.set_adjustment( adjustment );
#endif

  label.set_text( l.c_str() );
  scale.set_digits(0);
  if( sincr < 1 ) { scale.set_digits(1); spinButton.set_digits(1); }
  if( sincr < 0.1 )  { scale.set_digits(2); spinButton.set_digits(2); }
  scale.set_size_request( 140, -1 );
  spinButton.set_size_request( 50, -1 );
  spinButton.set_has_frame( false );

  //numentry.set_size_request( 30, -1 );
  numentry.set_has_frame( false );

  if( (max-min) < 1000000 ) {
    // Full widget with slider and spin button
    scale.set_value_pos(Gtk::POS_LEFT);
    scale.set_draw_value( false );
    align.set(0,0.5,0,1);
    align.add( label );

    //hbox.pack_start( scale, Gtk::PACK_SHRINK );
    //hbox.pack_start( spinButton, Gtk::PACK_SHRINK );
    //set_spacing(-3);
    //pack_start( align );

    vbox.set_spacing(-3);
    vbox.pack_start( align, Gtk::PACK_SHRINK );
    vbox.pack_start( scale, Gtk::PACK_SHRINK );
    //vbox2.pack_end( numentry, Gtk::PACK_SHRINK );
    //set_spacing(4);
    pack_start( vbox, Gtk::PACK_SHRINK );
    //pack_start( spinButton, Gtk::PACK_SHRINK );
    //hbox2.pack_start( vbox, Gtk::PACK_SHRINK );
    //hbox2.set_baseline_position( Gtk::BASELINE_POSITION_CENTER );
    hbox2.pack_start( numentry, Gtk::PACK_SHRINK, 4 );

    reset_button_align.set( Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER, 0, 0 );
    reset_button_align.add( reset_button );
    hbox2.pack_start( reset_button_align, Gtk::PACK_SHRINK );
    pack_start( hbox2, Gtk::PACK_SHRINK );
  } else {
    //hbox.pack_start( label, Gtk::PACK_SHRINK );
    //hbox.pack_start( spinButton, Gtk::PACK_SHRINK );
    pack_start( label, Gtk::PACK_SHRINK );
    //pack_start( spinButton, Gtk::PACK_SHRINK );
    pack_start( numentry, Gtk::PACK_SHRINK, 4 );
    reset_button_align.set( Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER, 0, 0 );
    reset_button_align.add( reset_button );
    pack_start( reset_button_align, Gtk::PACK_SHRINK );
  }

  //pack_start( hbox, Gtk::PACK_SHRINK );

#ifdef GTKMM_2
  adjustment.signal_value_changed().
    connect(sigc::mem_fun(*this,
        &PFWidget::changed));
#endif
#ifdef GTKMM_3
  adjustment->signal_value_changed().
    connect(sigc::mem_fun(*this,
        &PFWidget::changed));
#endif

  reset_button.signal_clicked.connect(sigc::mem_fun(*this,
        &PF::Slider::reset) );
  reset_button.signal_clicked.connect(sigc::mem_fun(*this,
        &PF::Slider::changed) );

  show_all_children();

}


PF::Slider::Slider( OperationConfigGUI* dialog, std::string pname, std::string l,
		    double val, double min, double max, double sincr, double pincr,
		    double mult ):
  PF::PFWidget( dialog, pname ),
#ifdef GTKMM_2
  adjustment( val, min, max, sincr, pincr, 0),
  scale(adjustment),
  spinButton(adjustment),
#endif
  reset_button(PF::PhotoFlow::Instance().get_data_dir()+"/icons/reset_active.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/reset_inactive.png"),
  multiplier(mult)
{
  create_widgets( l, val, min, max, sincr, pincr );
  value_changed.connect( sigc::mem_fun(*this, &PF::Slider::update_gui) );
}


PF::Slider::Slider( OperationConfigGUI* dialog, PF::ProcessorBase* processor, std::string pname, std::string l,
		    double val, double min, double max, double sincr, double pincr,
		    double mult ):
  PF::PFWidget( dialog, processor, pname ),
#ifdef GTKMM_2
  adjustment( val, min, max, sincr, pincr, 0),
  scale(adjustment),
  spinButton(adjustment),
#endif
  reset_button(PF::PhotoFlow::Instance().get_data_dir()+"/icons/reset_active.png",PF::PhotoFlow::Instance().get_data_dir()+"/icons/reset_inactive.png"),
  multiplier(mult)
{
  create_widgets( l, val, min, max, sincr, pincr );
  value_changed.connect( sigc::mem_fun(*this, &PF::Slider::update_gui) );
}


void PF::Slider::get_value()
{
  //std::cout<<"PF::Slider::get_value(): property=\""<<get_prop_name()<<"\"(0x"<<get_prop()<<")"<<std::endl;
  if( !get_prop() ) return;
  double val;
  get_prop()->get(val);
#ifdef GTKMM_2
  adjustment.set_value( val*multiplier );
#endif
#ifdef GTKMM_3
  adjustment->set_value( val*multiplier );
  spinButton.update();
  //std::cout<<"PF::Slider::get_value("<<get_prop_name()<<"): spinButton.value="<<spinButton.get_value()<<std::endl;
  //std::cout<<"PF::Slider::get_value("<<get_prop_name()<<"): adjustment.value="<<adjustment->get_value()<<std::endl;
#endif
}


void PF::Slider::set_value()
{
  if( !get_prop() ) return;
#ifdef GTKMM_2
  double val = adjustment.get_value()/multiplier;
#endif
#ifdef GTKMM_3
  double val = adjustment->get_value()/multiplier;
#endif
  get_prop()->update(val);
}