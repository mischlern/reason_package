 /* ------------------------------------------------------------------
  * GEM - Graphics Environment for Multimedia
  *
  *  Copyright (c) 2002 IOhannes m zmoelnig. forum::f�r::uml�ute. IEM
  *	zmoelnig@iem.kug.ac.at
  *  For information on usage and redistribution, and for a DISCLAIMER
  *  OF ALL WARRANTIES, see the file, "GEM.LICENSE.TERMS"
  *
  *  this file has been generated...
  * ------------------------------------------------------------------
  */

#ifndef INCLUDE_GEM_GLHINT_H_
#define INCLUDE_GEM_GLHINT_H_

#include "GemGLBase.h"

/*
 CLASS
	GEMglHint
 KEYWORDS
	openGL	0
 DESCRIPTION
	wrapper for the openGL-function
	"glHint( GLenum target, GLenum mode)"
 */

class GEM_EXTERN GEMglHint : public GemGLBase
{
	CPPEXTERN_HEADER(GEMglHint, GemGLBase)

	public:
	  // Constructor
	  GEMglHint (int, t_atom*); // CON

	protected:
	  // Destructor
	  virtual ~GEMglHint ();
	  // Do the rendering
	  virtual void	render (GemState *state);

	// variables
	  GLenum	target;		// VAR
	  virtual void	targetMess(t_atom);	// FUN

	  GLenum	mode;		// VAR
	  virtual void	modeMess(t_atom);	// FUN


	private:

	// we need some inlets
	  t_inlet *m_inlet[2];

	// static member functions
	  static void	 targetMessCallback (void*,t_symbol*,int,t_atom*);
	  static void	 modeMessCallback (void*,t_symbol*,int,t_atom*);
};
#endif // for header file