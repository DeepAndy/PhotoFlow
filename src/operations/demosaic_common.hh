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

#ifndef DEMOSAIC_COMMON_H
#define DEMOSAIC_COMMON_H

#include <string>

//#include <libraw/libraw.h>

#include "../base/operation.hh"
#include "../rt/rtengine/rawimagesource.hh"



namespace PF
{

void init_CFA(int filters, unsigned cfa[2][2]);


class PixelBuffer
{
  float** rows;
  float** ptr;
  int width, height;
public:
  PixelBuffer(float* buf, int width, int height, int rowstride, int roffs, int coffs);
  ~PixelBuffer();
  float** get_rows() { return rows; }
  float* operator[](int id) { return rows[id]; }
  int get_width() { return width; }
  int get_height() { return height; }
};



class DemosaicBasePar: public OpParBase
{
  dcraw_data_t* image_data;
  int border, border2;
  bool rgb_output;

public:
  DemosaicBasePar(int b, bool rgbout = true);
  bool has_intensity() { return false; }
  bool has_opacity() { return false; }
  bool needs_input() { return true; }

  dcraw_data_t* get_image_data() { return image_data; }

  int get_border() { return border; }

  void set_image_hints( VipsImage* img )
  {
    if( !img ) return;
    OpParBase::set_image_hints( img );
    if(rgb_output) rgb_image( get_xsize(), get_ysize() );
  }

  /* Function to derive the output area from the input area
   */
  virtual void transform(const VipsRect* rin, VipsRect* rout, int /*id*/)
  {
    rout->left = rin->left+border;
    rout->top = rin->top+border;
    rout->width = rin->width-border2;
    rout->height = rin->height-border2;
  }

  /* Function to derive the area to be read from input images,
       based on the requested output area
   */
  virtual void transform_inv(const VipsRect* rout, VipsRect* rin, int /*id*/)
  {
    // Output region aligned to the Bayer pattern
    int raw_left = (rout->left/2)*2;
    //if( raw_left < border ) raw_left = 0;

    int raw_top = (rout->top/2)*2;
    //if( raw_top < border ) raw_top = 0;

    int raw_right = rout->left+rout->width-1;
    //if( raw_right > (in->Xsize-border-1) ) raw_right = in->Xsize-1;

    int raw_bottom = rout->top+rout->height-1;
    //if( raw_bottom > (in->Ysize-border-1) ) raw_bottom = in->Ysize-1;

    int raw_width = raw_right-raw_left+1;
    int raw_height = raw_bottom-raw_top+1;

    rin->left = raw_left-border;
    rin->top = raw_top-border;
    rin->width = raw_width+border2;
    rin->height = raw_height+border2;

    if( (rin->width%2) ) rin->width += 1;
    if( (rin->height%2) ) rin->height += 1;
  }



  VipsImage* build(std::vector<VipsImage*>& in, int first,
      VipsImage* imap, VipsImage* omap,
      unsigned int& level);
};
}


#endif
