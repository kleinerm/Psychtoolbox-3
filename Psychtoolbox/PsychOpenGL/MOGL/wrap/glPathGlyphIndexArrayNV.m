function r = glPathGlyphIndexArrayNV( firstPathName, fontTarget, fontName, fontStyle, firstGlyphIndex, numGlyphs, pathParameterTemplate, emScale )

% glPathGlyphIndexArrayNV  Interface to OpenGL function glPathGlyphIndexArrayNV
%
% usage:  r = glPathGlyphIndexArrayNV( firstPathName, fontTarget, fontName, fontStyle, firstGlyphIndex, numGlyphs, pathParameterTemplate, emScale )
%
% C function:  GLenum glPathGlyphIndexArrayNV(GLuint firstPathName, GLenum fontTarget, const void* fontName, GLbitfield fontStyle, GLuint firstGlyphIndex, GLsizei numGlyphs, GLuint pathParameterTemplate, GLfloat emScale)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=8,
    error('invalid number of arguments');
end

r = moglcore( 'glPathGlyphIndexArrayNV', firstPathName, fontTarget, fontName, fontStyle, firstGlyphIndex, numGlyphs, pathParameterTemplate, emScale );

return
