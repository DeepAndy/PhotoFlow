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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../base/new_operation.hh"
#include "wavdec.hh"


PF::WavDecPar::WavDecPar():
OpParBase(),
numScales("numScales",this,0),
currScale("currScale",this,0),
preview_scale("preview_scale",this,1),
initial_lev("initial_lev",this,0),
blendFactor("blendFactor",this,0.5f)
{
  wavdec_algo = PF::new_wavdec_algo();

  set_type("wavdec" );
  set_default_name( _("Wavelet Decompose") );
}



void PF::WavDecPar::compute_padding( VipsImage* full_res, unsigned int id, unsigned int level )
{
  int padding = get_padding(numScales.get(), get_initial_lev());
  set_padding( padding, id );
}



VipsImage* PF::WavDecPar::build(std::vector<VipsImage*>& in, int first,
    VipsImage* imap, VipsImage* omap,
    unsigned int& level)
{
  if( (in.size()<1) || (in[0]==NULL) )
    return NULL;
  VipsImage* srcimg = in[0];
  
  std::vector<VipsImage*> in2;

  WavDecAlgoPar* wav_dec_par = dynamic_cast<WavDecAlgoPar*>( wavdec_algo->get_par() );
  int max_scales = wav_dec_par->get_maxScales(srcimg->Xsize, srcimg->Ysize);
  if (numScales.get() > max_scales) {
    std::cout<<"WavDecPar::build: max scale is "<<max_scales<<" for this image preview size"<<std::endl;
  }
  else {
    max_scales = numScales.get();
  }

  int padding = get_padding(max_scales, get_initial_lev());
  wav_dec_par->set_padding(padding);

//  std::cout<<"WavDecPar::build() padding: "<<padding<<std::endl;
  
  // Extend the original image by padding() pixels in order to decompose into scales
  VipsImage* extended;
  VipsExtend extend = VIPS_EXTEND_COPY;
  if( vips_embed(srcimg, &extended, padding, padding,
      srcimg->Xsize+padding*2, srcimg->Ysize+padding*2,
      "extend", extend, NULL) ) {
    std::cout<<"WavDecPar::build(): vips_embed() failed."<<std::endl;
    PF_REF( in[0], "WavDecPar::build(): vips_embed() failed." );
    return in[0];
  }
  
  // decompose the nscale
  wav_dec_par->set_numScales( get_numScales() );
  wav_dec_par->set_currScale( get_currScale() );
  wav_dec_par->set_blendFactor( get_blendFactor() );
  wav_dec_par->set_initial_lev( get_initial_lev() );
  wav_dec_par->set_preview_scale( get_preview_scale() );
  
  wav_dec_par->set_image_hints( extended );
  wav_dec_par->set_format( get_format() );
  in2.clear();
  in2.push_back( extended );
  VipsImage* wavdec = wav_dec_par->build( in2, 0, NULL, NULL, level );
  PF_UNREF( extended, "WavDecPar::build(): extended unref" );
  
  // crop the decomposed image to remove the padding pixels in order to return it
  VipsImage* cropped;
  if( vips_crop(wavdec, &cropped, padding, padding,
      srcimg->Xsize, srcimg->Ysize, NULL) ) {
    std::cout<<"WavDecPar::build(): vips_crop() failed."<<std::endl;
    PF_UNREF( wavdec, "WavDecPar::build(): wavdec unref" );
    PF_REF( in[0], "WavDecPar::build(): vips_crop() failed" );
    return in[0];
  }
  PF_UNREF( wavdec, "WavDecPar::build(): wavdec unref" );
  
  set_image_hints( cropped );
  
  return cropped;

}
