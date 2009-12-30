/*
 * libptbdrawtext_ftgl.cpp: Source of libptbdrawtext_ftgl dynamic shared library plugin
 * for Screen('DrawText') text rendering via external engine.
 *
 * This works on OS/X but is mostly intended for GNU/Linux.
 *
 * Features:
 * - Texture mapped renderer, like on OS/X with ATSU Drawtext.
 * - Fast due to texture object and display list caching for fast glyph recycling.
 * - Good text layoutting.
 * - Supports all Freetype2 supported fonts, e.g., vectorgraphics TrueType fonts.
 * - Anti-Aliased via Alpha-Blending.
 * - Unicode support.
 * - Text measuring.
 *
 * Building for OS/X:
 *
 * Needs libfreetype2, fontconfig, oglft, QT4 installed:
 *
 * g++ -framework QtCore -DQT_CORE_LIB -DQT_GUI_LIB -DOGLFT_QT_VERSION=4 -I /usr/local/include/OGLFT/ -I/opt/local/include/ -I /usr/X11R6/include/freetype2/ -I /Library/Frameworks/QtGui.framework/Headers/ -I /Library/Frameworks/QtCore.framework/Headers/ -L/opt/local/lib/ -framework OpenGL -l fontconfig -l oglft -l freetype -dynamiclib -o libptbdrawtext_ftgl.dylib libptbdrawtext_ftgl.cpp 
 *
 * Building for Linux:
 * TODO...
 */

// Standard libs for debug output:
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Include all GLFT and QT stuff:
#include "OGLFT.h"

// Include fontconfig stuff:
#include "fontconfig/fontconfig.h"
#include "fontconfig/fcfreetype.h"

int _antiAliasing = 1;
unsigned int _verbosity = 2;
bool _firstCall = true;
bool _needsRebuild = true;
bool _useOwnFontmapper = false;
unsigned _mapperFlags;
double _vxs, _vys, _vw, _vh;
char _fontName[4096] = { 0 };
unsigned int _fontStyle = 0;
char _fontFileName[4096] = { 0 };
int _faceIndex = 0;
double _fontSize = 0.0;
GLfloat _fgcolor[4];
GLfloat _bgcolor[4];

static OGLFT::TranslucentTexture *face = NULL;
static FT_Face ft_face = NULL;

extern "C" {

int PsychInitText(void);
int PsychShutdownText(void);
int PsychRebuiltFont(void);
int PsychSetTextFont(const char* fontName);
int PsychSetTextStyle(unsigned int fontStyle);
int PsychSetTextSize(double fontSize);
void PsychSetTextFGColor(double* color);
void PsychSetTextBGColor(double* color);
void PsychSetTextUseFontmapper(unsigned int useMapper, unsigned int mapperFlags);
void PsychSetTextViewPort(double xs, double ys, double w, double h);
int PsychDrawText(double xStart, double yStart, int textLen, double* text);
int PsychMeasureText(int textLen, double* text, float* xmin, float* ymin, float* xmax, float* ymax);
void PsychSetTextVerbosity(unsigned int verbosity);
void PsychSetTextAntiAliasing(int antiAliasing);

void PsychSetTextVerbosity(unsigned int verbosity)
{
	_verbosity = verbosity;
	return;
}

void PsychSetTextAntiAliasing(int antiAliasing)
{
	_antiAliasing = antiAliasing;
	return;
}

int PsychRebuildFont(void)
{
	// Destroy old font object, if any:
	if (face) {
		// Delete OGLFT face object:
		delete(face);
		face = NULL;
		
		if (_verbosity > 3) fprintf(stderr, "libptbdrawtext_ftgl: Destroying old font face...\n");
		
		// Delete underlying FreeType representation:
		FT_Done_Face(ft_face);
		ft_face = NULL;
	}

	if (_useOwnFontmapper) {
		FcResult result;
		FcPattern* target = NULL;
		
		if (_fontName[0] == '-') {
			// _fontName starts with a '-' dash: This is not a simple font family string but a special
			// fontspec string in FontConfig's special format. It contains many possible required font
			// properties encoded in the string. Parse it into a font matching pattern:
			target = FcNameParse((FcChar8*) &(_fontName[1]));
		}
		else {
			// _fontName contains only font family name: Build matching pattern based on _fontSize and
			// the flags provided in _fontStyle, according to the conventions in Psychtoolbox Screen('TextStyle'):
			target = FcPatternBuild (0, FC_FAMILY, FcTypeString, _fontName, FC_PIXEL_SIZE, FcTypeDouble, _fontSize,
									 FC_WEIGHT, FcTypeInteger, ((_fontStyle & 1) ? FC_WEIGHT_BOLD : FC_WEIGHT_NORMAL),
									 FC_SLANT, FcTypeInteger, ((_fontStyle & 2) ?  FC_SLANT_ITALIC : FC_SLANT_ROMAN),
									 FC_OUTLINE, FcTypeBool, ((_fontStyle & 8) ? true : false),
									 FC_WIDTH, FcTypeInteger, ( (_fontStyle & 32) ?  FC_WIDTH_CONDENSED : ((_fontStyle & 64) ?  FC_WIDTH_EXPANDED : FC_WIDTH_NORMAL) ),
									 FC_DPI, FcTypeDouble, (double) 72.0,
									 FC_SCALABLE, FcTypeBool, true,
									 NULL);
		}
		
		// Have a matching pattern:
		if (_verbosity > 3) {
			fprintf(stderr, "libptbdrawtext_ftgl: Trying to find font that closely matches following specification:\n");
			FcPatternPrint(target);
		}

		// Perform font matching for the font in the default configuration (0) that best matches the
		// specified target pattern:
		FcPattern* matched = FcFontMatch(0, target, &result);
		if (result == FcResultNoMatch) {
			// Failed!
			if (_verbosity > 1) fprintf(stderr, "libptbdrawtext_ftgl: FontConfig failed to find a matching font for family %s, size %f pts and style flags %i.\n", _fontName, (float) _fontSize, _fontStyle);
			return(1);
		}

		// Success: Extract relevant information for Freetype-2, the font filename and faceIndex:
		if (_verbosity > 3) {
			fprintf(stderr, "libptbdrawtext_ftgl: Best matching font which will be selected for drawing has following specs:\n");
			FcPatternPrint(matched);
		}

		// Retrieve font filename for matched font:
		FcChar8* localfontFileName = NULL;
		if (FcPatternGetString(matched, FC_FILE, 0, (FcChar8**) &localfontFileName) != FcResultMatch) {
			// Failed!
			if (_verbosity > 1) fprintf(stderr, "libptbdrawtext_ftgl: FontConfig did not find filename for font with family %s, size %f pts and style flags %i.\n", _fontName, (float) _fontSize, _fontStyle);
			return(1);
		}

		strcpy(_fontFileName, (char*) localfontFileName);

		// Retrieve faceIndex within fontfile:
		if (FcPatternGetInteger(matched, FC_INDEX, 0, &_faceIndex) != FcResultMatch)  {
			// Failed!
			if (_verbosity > 1) fprintf(stderr, "libptbdrawtext_ftgl: FontConfig did not find faceIndex for font file %s, family %s, size %f pts and style flags %i.\n", _fontFileName, _fontName, (float) _fontSize, _fontStyle);
			return(1);
		}

		// Release target pattern and matched pattern objects:
		FcPatternDestroy(target);
		FcPatternDestroy(matched);
	}
	else {
		// Use "raw" values as passed by calling client code:
		strcpy(_fontFileName, _fontName);
		_faceIndex = (int) _fontStyle;
	}
	
	// Load & Create new font and face object, based on current spec settings:
	// We directly use the Freetype library, so we can spec the faceIndex for selection of textstyle, which wouldn't be
	// possible with the higher-level OGLFT constructor...
    FT_Error error = FT_New_Face( OGLFT::Library::instance(), _fontFileName, _faceIndex, &ft_face );
	if (error) {
		if (_verbosity > 1) fprintf(stderr, "libptbdrawtext_ftgl: Freetype did not load face with index %i from font file %s.\n", _faceIndex, _fontFileName);
		return(1);
	}
	else {
		if (_verbosity > 3) fprintf(stderr, "libptbdrawtext_ftgl: Freetype loaded face %p with index %i from font file %s.\n", ft_face, _faceIndex, _fontFileName);
	}

	// Create FTGL face from Freetype face with given size and a 72 DPI resolution, aka _fontSize == pixelsize:
	face = new OGLFT::TranslucentTexture(ft_face, _fontSize, 72);
	
	// Test the created face to make sure it will work correctly:
	if (!face->isValid()) {
		if (_verbosity > 1) fprintf(stderr, "libptbdrawtext_ftgl: Freetype did not recognize %s as a font file.\n", _fontName);
		return(1);
	}
	
	// Ready!
	_needsRebuild = false;
	
	return(0);
}

void PsychSetTextFGColor(double* color)
{
	_fgcolor[0] = color[0];
	_fgcolor[1] = color[1];
	_fgcolor[2] = color[2];
	_fgcolor[3] = color[3];

	return;
}

void PsychSetTextBGColor(double* color)
{
	_bgcolor[0] = color[0];
	_bgcolor[1] = color[1];
	_bgcolor[2] = color[2];
	_bgcolor[3] = color[3];
	
	return;
}

// Assign dimensions and position of target viewport for rendering:
// Assumption is that origin is bottom-left, x-axis pointing right, y-axis pointint up.
// The plugin needs to remap or setup proper OpenGL transformations if its underlying
// renderbackend needs a different geometric setup:
void PsychSetTextViewPort(double xs, double ys, double w, double h)
{
	_vxs = xs;
	_vys = ys;
	_vw  = w;
	_vh  = h;

	return;
}

// Should the plugin use its own fontMapper implementation, or can it rely at least
// partially - as defined by mapperFlags - on the PTB internal mapper?
void PsychSetTextUseFontmapper(unsigned int useMapper, unsigned int mapperFlags)
{
	_useOwnFontmapper = (useMapper > 0) ? true : false;
	_mapperFlags      = mapperFlags;
	
	return;
}

// Select font Name:
int PsychSetTextFont(const char* fontName)
{
	if (strcmp(_fontName, fontName)) _needsRebuild = true;
	strcpy(_fontName, fontName);
	return(0);
}

// Select font Style:
int PsychSetTextStyle(unsigned int fontStyle)
{
	if (_fontStyle != fontStyle) _needsRebuild = true;
	_fontStyle = fontStyle;
	return(0);
}

// Select font Size:
int PsychSetTextSize(double fontSize)
{
	if (_fontSize != fontSize) _needsRebuild = true;
	_fontSize = fontSize;
	return(0);
}

int PsychDrawText(double xStart, double yStart, int textLen, double* text)
{
	int i;
	GLuint ti;
	QString	uniCodeText;
	
	// On first invocation after init we need to generate a useless texture object.
	// This is a weird workaround for some weird bug somewhere in FTGL...
	if (_firstCall) {
		_firstCall = false;
		glGenTextures(1, &ti);
	}
	
	// Check if rebuild of font face needed due to parameter
	// change. Reload/Rebuild font face if so, check for errors:
	if (_needsRebuild && PsychRebuildFont()) return(1);

	// Synthesize Unicode QString from double vector:
	for(i = 0; i < textLen; i++) {
		uniCodeText += QChar((unsigned int) text[i]);
	}
	
	glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
	glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1);
    glEnable( GL_TEXTURE_2D );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(_vxs, _vxs + _vw, _vys, _vys + _vh);
	glMatrixMode(GL_MODELVIEW);
	
	// Set text color: This will be filtered by OGLFT for redundant settings:
	face->setForegroundColor( _fgcolor[0], _fgcolor[1], _fgcolor[2], _fgcolor[3]);

	// Rendering of background quad requested? -- True if background alpha > 0.
	if (_bgcolor[3] > 0) {
		// Yes. Compute bounding box of "to be drawn" text and render a quad in background color:
		float xmin, ymin, xmax, ymax;
		PsychMeasureText(textLen, text, &xmin, &ymin, &xmax, &ymax);
		glColor4fv(&(_bgcolor[0]));
		glRectf(xmin + xStart, ymin + yStart, xmax + xStart, ymax + yStart);
	}
	
	// Draw the text at selected start location:
	face->draw(xStart, yStart, uniCodeText);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
    glDisable( GL_TEXTURE_2D );
	glPopAttrib();
	glPopClientAttrib();
	
	// Ready!
	return(0);	
}

int PsychMeasureText(int textLen, double* text, float* xmin, float* ymin, float* xmax, float* ymax)
{
	int i;
	QString	uniCodeText;
	
	// Check if rebuild of font face needed due to parameter
	// chage. Reload/Rebuild font face if so, check for errors:
	if (_needsRebuild && PsychRebuildFont()) return(1);

	// Synthesize Unicode QString from double vector:
	for(i = 0; i < textLen; i++) {
		uniCodeText += QChar((unsigned int) text[i]);
	}

	// Compute its bounding box:
	OGLFT::BBox box = face->measure(uniCodeText);
	
	*xmin = box.x_min_;
	*ymin = box.y_min_;
	*xmax = box.x_max_;
	*ymax = box.y_max_;

	return(0);
}

int PsychInitText(void)
{
	_firstCall = true;
	_needsRebuild = true;
	face = NULL;

	// Try to initialize libfontconfig - our fontMapper library for font matching and selection:
	if (!FcInit()) {
		if (_verbosity > 0) fprintf(stderr, "libptbdrawtext_ftgl: FontMapper initialization failed!\n");
		return(1);
	}
	
	if (_verbosity > 3)	fprintf(stderr, "libptbdrawtext_ftgl: Initialized.\n");
	return(0);
}

int PsychShutdownText(void)
{
	if (face) {
		delete(face);
		face = NULL;
		FT_Done_Face(ft_face);
		ft_face = NULL;
		if (_verbosity > 3) fprintf(stderr, "libptbdrawtext_ftgl: Shutting down.\n");
	}
	
	_needsRebuild = true;
	_firstCall = false;
	
	// Shutdown fontmapper library:
	FcFini();

	return(0);
}

} // extern "C"
