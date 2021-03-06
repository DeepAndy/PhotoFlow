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

#ifndef LOCAL_CONTRAST_V2_CONFIG_DIALOG_HH
#define LOCAL_CONTRAST_V2_CONFIG_DIALOG_HH

#include <gtkmm.h>

#include "../operation_config_gui.hh"
#include "../../operations/local_contrast_v2.hh"


namespace PF {

  class LocalContrastV2ConfigGUI: public OperationConfigGUI
  {
    Gtk::VBox controlsBox;
    Gtk::VBox controlsBox2;
    Gtk::HBox equalizerBox;
    Gtk::HBox globalBox;
    Gtk::HBox equalizerCheckboxBox;
    Gtk::Alignment equalizerCheckboxPadding;
    Gtk::Alignment equalizerPadding;
    Gtk::HSeparator separator;
    
    Slider amount_slider;
    Slider white_level_slider;
    Slider boost_slider;

    Gtk::VBox guidedControlsBox;
    Slider guidedRadiusSlider;
    Slider guidedThresholdSlider;

  public:
    LocalContrastV2ConfigGUI( Layer* l );
    
    bool has_preview() { return true; }

    void do_update();
  };

}

#endif
