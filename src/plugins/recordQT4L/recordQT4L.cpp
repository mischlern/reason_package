////////////////////////////////////////////////////////
//
// GEM - Graphics Environment for Multimedia
//
// zmoelnig@iem.kug.ac.at
//
// Implementation file 
//
//    Copyright (c) 1997-1999 Mark Danks.
//    Copyright (c) G�nther Geiger.
//    Copyright (c) 2001-2002 IOhannes m zmoelnig. forum::f�r::uml�ute. IEM
//    For information on usage and redistribution, and for a DISCLAIMER OF ALL
//    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.
//
/////////////////////////////////////////////////////////
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "recordQT4L.h"
using namespace gem;
#include "Base/GemBase.h"
#include "Base/GemMan.h"

#include <stdlib.h>


#ifdef  GEM_USE_RECORDQT4L
REGISTER_RECORDFACTORY("QT4L", recordQT4L);
#endif
/////////////////////////////////////////////////////////
//
// recordQT4L
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////

recordQT4L :: recordQT4L(): 
  record()
#if defined  GEM_USE_RECORDQT4L
  ,
  m_qtfile(NULL),
  m_codec(NULL), m_codecs(NULL),
  m_codecname(NULL), m_codecnum(-1),
  m_qtbuffer(NULL),
  m_colormodel(0),
  m_restart(true)
{
  //  strcpy(m_compressor, QUICKTIME_DV);
  lqt_registry_init ();
}
#else
{}
#endif

////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
recordQT4L :: ~recordQT4L()
{
  close();
}

#if defined  GEM_USE_RECORDQT4L
void recordQT4L :: close(void)
{
  if(m_qtfile){
    quicktime_close(m_qtfile);
    m_qtfile=NULL;
  }
#if 0
  /* this crashes badly! */
  if(m_qtbuffer) {
     lqt_rows_free(m_qtbuffer);
  }
#endif
}

/////////////////////////////////////////////////////////
// open a file !
//
/////////////////////////////////////////////////////////
static struct
{
  const char * name;
  const lqt_file_type_t type;
  const char * extension;
  const char * description;
  const char * default_video_codec;
} qtformats[] =  {
  { "qt",       LQT_FILE_QT,        "mov", "Quicktime (QT7 compatible)",   "yuv2" }, /* ffmpeg_mpg4 */
  { "qtold",    LQT_FILE_QT_OLD,    "mov", "Quicktime (qt4l and old lqt)", "yuv2" }, /* mjpa */
  { "avi",      LQT_FILE_AVI,       "avi", "AVI (< 2G)",                   "yuv2" }, /* ffmpeg_msmpeg4v3 */
  { "avi_odml", LQT_FILE_AVI_ODML,  "avi", "AVI (> 2G)",                   "yuv2" }, /* ffmpeg_msmpeg4v3 */
  { "mp4",      LQT_FILE_MP4,       "mp4", "ISO MPEG-4",                   "yuv2" }, /* ffmpeg_mpg4 */
  { "m4a",      LQT_FILE_M4A,       "m4a", "m4a (iTunes compatible)",      "yuv2"  }, /* ffmpeg_mpg4 */
};
/* guess the file-format by inspecting the extension */
static lqt_file_type_t guess_qtformat(const std::string filename)
{
  const char * extension = strrchr(filename.c_str(), '.');
  unsigned int i=0;

  if(!extension) {
    error("no extension given: encoding will be QuickTime");
    return LQT_FILE_QT;
  }

  extension++;

  for(i = 0; i < sizeof(qtformats)/sizeof(qtformats[0]); i++) {
    if(!strcasecmp(extension, qtformats[i].extension)) {
      post("encoding as %s", qtformats[i].description);
      return qtformats[i].type;
    }
  }
  
  error("unknown extension: encoding will be QuickTime");
  return LQT_FILE_QT; /* should be save for now */
}

bool recordQT4L :: open(const std::string filename)
{
  close();

  lqt_file_type_t type =  guess_qtformat(filename);

  m_qtfile = lqt_open_write(filename.c_str(), type);
  if(m_qtfile==NULL){
    return false;
  }

  m_restart=true;
  return (true);
}

/////////////////////////////////////////////////////////
// initialize the encoder
//
/////////////////////////////////////////////////////////
bool recordQT4L :: init(const imageStruct*img, const int framedur)
{
  int rowspan=0, rowspan_uv=0;
  lqt_codec_info_t*codec=NULL;

  if(!m_qtfile || !img || framedur < 0)
    return false;

  /* do we have a codec specified? */
  const char*codecname = m_codecname;
  if(m_codecnum>=0) {
    lqt_codec_info_t**codecs = lqt_query_registry(0,1,1,0);
    if(!setCodec(codecs, m_codecnum)) {
      error("couldn't initialize codec#%d", m_codecnum);
      return false;
    }

    codec=m_codec;
    m_codec=NULL; /* next time we init, we want to get the default codec again */
  }

  if(NULL==codec){
    if(NULL==codecname) {
      /* LATER figure out automatically which codec to use */ 
      lqt_file_type_t type = lqt_get_file_type(m_qtfile);
      unsigned int i=0;
      for(i = 0; i < sizeof(qtformats)/sizeof(qtformats[0]); i++) {
        if(type == qtformats[i].type)
          {
            codecname = qtformats[i].default_video_codec;
          }
      }
      if(NULL==codecname) {
        error("couldn't find default codec for this format");
        return false;
      }
    }
  
    lqt_codec_info_t**codecs = (lqt_codec_info_t**)lqt_find_video_codec_by_name(codecname);
  
    if(!setCodec(codecs, 0)){
      error("couldn't initialize default codec: %s", codecname);
      return false;
    }
    
    verbose(1, "using codec: %s", codecname);
  
    codec=m_codec;
    m_codec=NULL; /* next time we init, we want to get the default codec again */
  }

  if(NULL==codec) {
    error("couldn't initialize codec");
    return false;
  }
  
  /* fps = time_scale / frame_duration */
  lqt_set_video(m_qtfile,
                1, 
                img->xsize, 
                img->ysize,
                1,
                framedur,
                codec);

  /* set the colormodel */
  m_colormodel=BC_RGB888; /* LATER do this more dynamically */

  /* make sure to allocate enough buffer; it sometimes crashes when i allocate the "right" size, 
     so we just grab a multiple of what we actually want... 
  */
  /* but isn't this a memleak? it sure crashes if i try to lqt_rows_free() the qtbuffer */
  m_qtbuffer = lqt_rows_alloc(2*img->xsize, 2*img->ysize, m_colormodel, &rowspan, &rowspan_uv);

  quicktime_set_cmodel(m_qtfile, m_colormodel);

  m_width =img->xsize;
  m_height=img->ysize;

  return true;
}

/////////////////////////////////////////////////////////
// do the actual encoding and writing to file
//
/////////////////////////////////////////////////////////
bool recordQT4L :: putFrame(imageStruct*img)
{
  if(!m_qtfile || !img){
    return false;
  }
  unsigned char**rowpointers;
  int row, row_stride;
  float framerate = GemMan::getFramerate();

  if(m_width!=img->xsize || m_height!=img->ysize)m_restart=true;

  if(m_restart){
    if(!init(img, static_cast<int>(framerate))) {
      /* something went wrong! */
      close();
      error("unable to initialize QT4L");
      return false;
    }
    m_restart=false;
  }

  switch(m_colormodel){
  case BC_RGBA8888:
    m_image.convertFrom(img, GL_RGBA);
    break;
  case BC_RGB888:
    m_image.convertFrom(img, GL_RGB);
    break;
  case BC_YUV422:
    m_image.convertFrom(img, GL_YUV422_GEM);
    break;
  default:
    error("record: unsupported colormodel...");
    return false;
  }

  row=m_image.ysize;
  row_stride=m_image.xsize*m_image.csize;
  rowpointers=new unsigned char*[row];
  if(!m_image.upsidedown){
    while(row--){
      rowpointers[m_image.ysize-row-1]=m_image.data+(row-1)*row_stride;
    }
  } else {
    while(row--){
      rowpointers[row]=m_image.data+row*row_stride;
    }
  }

  lqt_encode_video(m_qtfile, rowpointers, 0, static_cast<int>(framerate));
  delete[]rowpointers;
  return true;
}



/////////////////////////////////////////////////////////
// get codecs
//
/////////////////////////////////////////////////////////
std::vector<std::string>recordQT4L::getCodecs() {
  std::vector<std::string>result;
  m_codecdescriptions.clear();

  lqt_codec_info_t**codec = lqt_query_registry(0,1,1,0);

  if(codec) {
    int n=0;
    while(NULL!=codec[n]){
      std::string name=codec[n]->name;
      std::string desc=codec[n]->long_name;
      result.push_back(name);
      m_codecdescriptions[name]=desc;

      n++;
    }

    lqt_destroy_codec_info(codec);
  }

  return result;
}


/////////////////////////////////////////////////////////
// set qt-codec
/////////////////////////////////////////////////////////
bool recordQT4L :: setCodec(lqt_codec_info_t**codec, int num)
{
  int n=0;

  if(!codec || num<0) /* somebody wants to trick us! */
    return false;

  /* check whether we have at least 'num' codecs to select from */
  while(codec[n])n++;

  if(num<0 || num>=n || !codec[num]) {
    /* 'codec' does not hold the requested 'num' */
    lqt_destroy_codec_info(codec);
    return false;
  }

  /* OK, selection is valid: use the new codec information rather than the old one*/

  /* clean up the old codecs */
  if(m_codecs && m_codecs!=codec)
    lqt_destroy_codec_info(m_codecs);

  /* prepare to use the new codecs */
  m_codecs=codec;
  m_codec=codec[num];
  m_width=m_height=-1;

  return true;
}
/////////////////////////////////////////////////////////
// set codec by number
//
/////////////////////////////////////////////////////////
bool recordQT4L :: setCodec(int num)
{
  m_codecname=NULL;
  m_codecnum=num;
  return true;
}
/////////////////////////////////////////////////////////
// set codec by name
//
/////////////////////////////////////////////////////////
bool recordQT4L :: setCodec(const char*name)
{
  m_codecname=name;
  m_codecnum=-1;
  return true;
}
#endif