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

#pragma once

#include <assert.h>
#include <string>

#include <sstream>
//#define SSTR( x ) dynamic_cast< std::ostringstream & >( \
//        ( std::ostringstream() << std::dec << x ) ).str()

#include <OpenColorIO/OpenColorIO.h>
namespace OCIO = OCIO_NAMESPACE;



#include "../../base/processor.hh"


namespace PF 
{

enum ACESview_t
{
  ACES_VIEW_sRGB,
  ACES_VIEW_DCDM,
  ACES_VIEW_DCDM_P3_gamut_clip,
  ACES_VIEW_P3_D60,
  ACES_VIEW_P3_D60_ST2084_1000_nits,
  ACES_VIEW_P3_D60_ST2084_2000_nits,
  ACES_VIEW_P3_D60_ST2084_4000_nits,
  ACES_VIEW_P3_DCI,
  ACES_VIEW_Rec_2020,
  ACES_VIEW_Rec_2020_ST2084_1000_nits,
  ACES_VIEW_Rec_709,
  ACES_VIEW_Rec_709_D60_sim,
  ACES_VIEW_sRGB_D60_sim,
  ACES_VIEW_RAW,
  ACES_VIEW_LOG
};

  class OCIOACESPar: public OpParBase
  {
    OCIO::ConstConfigRcPtr config;

    std::string displayName;
    std::string viewName;
    std::string displayColorSpace;

    PropertyBase view_name;
    ProcessorBase* convert2ACES;

    OCIO::DisplayTransformRcPtr transform;
    OCIO::ConstProcessorRcPtr processor;

  public:
    OCIOACESPar();
    ~OCIOACESPar() {
    }

    bool has_intensity() { return false; }
    bool has_opacity() { return false; }

    OCIO::ConstProcessorRcPtr get_processor() { return processor; }

    VipsImage* build(std::vector<VipsImage*>& in, int first, 
		     VipsImage* imap, VipsImage* omap, 
		     unsigned int& level);
  };

  

  template < OP_TEMPLATE_DEF > 
  class OCIOACESProc
  {
  public: 
    void render(VipsRegion** ireg, int n, int in_first,
		VipsRegion* imap, VipsRegion* omap, 
		VipsRegion* oreg, OpParBase* par)
    {
      if( n != 1 ) return;
      if( ireg[0] == NULL ) return;

      OCIOACESPar* opar = dynamic_cast<OCIOACESPar*>(par);
      if( !opar ) return;

      if( ireg[0]->im->Bands != 3 ) return;

      int es = VIPS_IMAGE_SIZEOF_ELEMENT( ireg[0]->im );
      int ips = VIPS_IMAGE_SIZEOF_PEL( ireg[0]->im );
      const int ops = VIPS_IMAGE_SIZEOF_PEL( oreg->im );

      int offsx = ireg[0]->valid.left;
      int offsy = ireg[0]->valid.top;
      int rw = ireg[0]->valid.width;
      int rh = ireg[0]->valid.height;
      int rowstride = rw*ireg[0]->im->Bands;

      if(!opar->get_processor()) return;

			
      float* buf = new float[rowstride*rh];
      int x, y, z;

      for(y = 0; y < rh; y++) {
        float* p = (float*)VIPS_REGION_ADDR( ireg[0], offsx, offsy+y );
        memcpy(buf+rowstride*y, p, rowstride*es);
      }
      OCIO::PackedImageDesc img(buf, rw, rh, 3);
      opar->get_processor()->apply(img);

      offsx = oreg->valid.left;
      offsy = oreg->valid.top;
      rw = oreg->valid.width;
      rh = oreg->valid.height;
      int orowstride = rw*oreg->im->Bands;
      for(y = 0; y < rh; y++) {
        float* p = (float*)VIPS_REGION_ADDR( oreg, offsx, offsy+y );
        memcpy(p, buf+rowstride*y, orowstride*es);
        //memset(p, 0, orowstride*es);
      }
      delete[] buf;
    }
  };




  ProcessorBase* new_ocio_aces();
}


