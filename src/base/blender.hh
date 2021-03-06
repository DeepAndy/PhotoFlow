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


#ifndef BLENDER_HH
#define BLENDER_HH


#include "pftypes.hh"


namespace PF 
{


#define BLEND_LOOP( theblender ) {                                \
    theblender.init_line( omap, r->left, y0 );                    \
    for( x=0, xomap=0; x < line_size; ) {                         \
      for( ch=0; ch<CHMIN; ch++, x++ ) pout[x] = pbottom[x];      \
      theblender.blend( opacity, pbottom, tmpline, pout, x, xomap );       \
      x += dx;                                                         \
      for( ch=CHMAX+1; ch<NCH; ch++, x++ ) pout[x] = pbottom[x]; \
    }                                                                   \
  }


#define BLEND_LOOP2( theblender ) {                               \
    theblender.init_line( omap, r->left, y0 );                    \
    for( x=0, xomap=0; x < line_size; ) {                         \
      theblender.blend( opacity, pbottom, tmpline, pout, x, xomap ); \
      x += NCH;                   \
    }                                                             \
  }


  
  template<typename T, colorspace_t colorspace, int CHMIN, int CHMAX, bool has_omap>
  class Blender
  {
    int mode;
    float opacity;
    cmsHTRANSFORM transform;
    ICCProfile* data;
    ICCTransform* img2lab;
    ICCTransform* lab2img;

    BlendNormal<T,colorspace,CHMIN,CHMAX,has_omap> blend_normal;
    BlendAdd<T,colorspace,CHMIN,CHMAX,has_omap> blend_add;
    BlendSubtract<T,colorspace,CHMIN,CHMAX,has_omap> blend_subtract;
    BlendGrainExtract<T,colorspace,CHMIN,CHMAX,has_omap> blend_grain_extract;
    BlendGrainMerge<T,colorspace,CHMIN,CHMAX,has_omap> blend_grain_merge;
    BlendMultiply<T,colorspace,CHMIN,CHMAX,has_omap> blend_multiply;
    BlendDivide<T,colorspace,CHMIN,CHMAX,has_omap> blend_divide;
    BlendScreen<T,colorspace,CHMIN,CHMAX,has_omap> blend_screen;
    BlendLighten<T,colorspace,CHMIN,CHMAX,has_omap> blend_lighten;
    BlendDarken<T,colorspace,CHMIN,CHMAX,has_omap> blend_darken;
    BlendExclusion<T,colorspace,CHMIN,CHMAX,has_omap> blend_exclusion;
    BlendOverlay<T,colorspace,CHMIN,CHMAX,has_omap> blend_overlay;
    BlendOverlayGimp<T,colorspace,CHMIN,CHMAX,has_omap> blend_overlay_gimp;
    BlendSoftLight<T,colorspace,CHMIN,CHMAX,has_omap> blend_soft_light;
    BlendHardLight<T,colorspace,CHMIN,CHMAX,has_omap> blend_hard_light;
    BlendVividLight<T,colorspace,CHMIN,CHMAX,has_omap> blend_vivid_light;
    BlendLuminosity<T,colorspace,CHMIN,CHMAX,has_omap> blend_lumi;
    BlendLuminance<T,colorspace,CHMIN,CHMAX,has_omap> blend_luminance;
    BlendColor<T,colorspace,CHMIN,CHMAX,has_omap> blend_color;
    BlendLCH<T,colorspace,CHMIN,CHMAX,has_omap> blend_lch;

  public:
    Blender( int m, float o ):
      mode( m ), opacity( o ), transform( NULL ), data( NULL )
    {
    }
    
    void set_transform(cmsHTRANSFORM t) { transform = t; }
    void set_icc_data( ICCProfile* d )
    {
      data = d;
      blend_lumi.set_icc_data(data);
    }
    void set_img2lab_transform(ICCTransform* t)
    {
      img2lab = t;
      blend_lch.set_img2lab_transform(t);
    }
    void set_lab2img_transform(ICCTransform* t)
    {
      lab2img = t;
      blend_lch.set_lab2img_transform(t);
    }

    void blend(VipsRegion* bottom, VipsRegion* top, VipsRegion* oreg, VipsRegion* omap) 
    {
      if( !bottom || !top ) return;
      VipsRect *r = &oreg->valid;
      //int x, y, xomap, y0, dx1=CHMIN, dx2=PF::ColorspaceInfo<colorspace>::NCH-CHMIN, ch, CHMAXplus1=CHMAX+1;
      int x, y, xomap, y0, dx=CHMAX-CHMIN+1, ch;
      int width = r->width;
      int NCH = oreg->im->Bands;
      int line_size = width * oreg->im->Bands;
      T* pbottom;
      T* ptop;
      T* pout;
      T* line = NULL;
      if( transform ) line = new T[line_size];
      T* tmpline = NULL;
      for( y = 0; y < r->height; y++ ) {      
        y0 = r->top + y;
        pbottom = (T*)VIPS_REGION_ADDR( bottom, r->left, y0 ); 
        ptop = (T*)VIPS_REGION_ADDR( top, r->left, y0 ); 
        pout = (T*)VIPS_REGION_ADDR( oreg, r->left, y0 ); 
        if( transform ) {
          tmpline = line;
          cmsDoTransform( transform, ptop, tmpline, width );
          //std::cout<<"Blender::blend(): calling cmsDoTransform()"<<std::endl;
        } else {
          tmpline = ptop;
        }
        switch(mode) {
        case PF_BLEND_PASSTHROUGH:
          break;
        case PF_BLEND_NORMAL:
        //case PF_MASK_BLEND_NORMAL:
          BLEND_LOOP(blend_normal);
          break;
        case PF_BLEND_ADD:
          BLEND_LOOP(blend_add);
          break;
        case PF_BLEND_SUBTRACT:
          BLEND_LOOP(blend_subtract);
          break;
        case PF_BLEND_GRAIN_EXTRACT:
          BLEND_LOOP(blend_grain_extract);
          break;
        case PF_BLEND_GRAIN_MERGE:
          BLEND_LOOP(blend_grain_merge);
          break;
        case PF_BLEND_OVERLAY:
          BLEND_LOOP(blend_overlay);
          break;
        case PF_BLEND_OVERLAY_GIMP:
          BLEND_LOOP(blend_overlay_gimp);
          break;
        case PF_BLEND_SOFT_LIGHT:
          BLEND_LOOP(blend_soft_light);
          break;
        case PF_BLEND_HARD_LIGHT:
          BLEND_LOOP(blend_hard_light);
          break;
        case PF_BLEND_VIVID_LIGHT:
          BLEND_LOOP(blend_vivid_light);
          break;
        case PF_BLEND_MULTIPLY:
          BLEND_LOOP(blend_multiply);
          break;
        case PF_BLEND_DIVIDE:
          BLEND_LOOP(blend_divide);
          break;
        case PF_BLEND_SCREEN:
          BLEND_LOOP(blend_screen);
          break;
        case PF_BLEND_LIGHTEN:
        case PF_MASK_BLEND_UNION:
          BLEND_LOOP(blend_lighten);
          break;
        case PF_BLEND_DARKEN:
        case PF_MASK_BLEND_INTERSECTION:
          BLEND_LOOP(blend_darken);
          break;
        case PF_MASK_BLEND_EXCLUSION:
          BLEND_LOOP(blend_exclusion);
          break;
        case PF_BLEND_LUMI:
          blend_lumi.set_icc_data( data );
          BLEND_LOOP2(blend_lumi);
          break;
        case PF_BLEND_LUMINANCE:
          blend_luminance.set_icc_data( data );
          BLEND_LOOP2(blend_luminance);
          break;
        case PF_BLEND_COLOR:
          blend_color.set_icc_data( data );
          BLEND_LOOP2(blend_color);
          break;
        case PF_BLEND_LCH_L:
          //blend_lch.set_icc_data( data, oreg->im->BandFmt );
          blend_lch.set_channel(0);
          BLEND_LOOP2(blend_lch);
          break;
        case PF_BLEND_LCH_C:
          //blend_lch.set_icc_data( data, oreg->im->BandFmt );
          blend_lch.set_channel(1);
          BLEND_LOOP2(blend_lch);
          break;
        case PF_BLEND_LCH_H:
          //blend_lch.set_icc_data( data, oreg->im->BandFmt );
          blend_lch.set_channel(2);
          BLEND_LOOP2(blend_lch);
          break;
        case PF_BLEND_LAST:
          break;
        default:
          break;
        }
      }
      if(line) delete[] line;
    }
  };



}


#endif
