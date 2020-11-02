function r = glPathMemoryGlyphIndexArrayNV( firstPathName, fontTarget, fontSize, fontData, faceIndex, firstGlyphIndex, numGlyphs, pathParameterTemplate, emScale )

% glPathMemoryGlyphIndexArrayNV  Interface to OpenGL function glPathMemoryGlyphIndexArrayNV
%
% usage:  r = glPathMemoryGlyphIndexArrayNV( firstPathName, fontTarget, fontSize, fontData, faceIndex, firstGlyphIndex, numGlyphs, pathParameterTemplate, emScale )
%
% C function:  GLenum glPathMemoryGlyphIndexArrayNV(GLuint firstPathName, GLenum fontTarget, GLsizeiptr fontSize, const void* fontData, GLsizei faceIndex, GLuint firstGlyphIndex, GLsizei numGlyphs, GLuint pathParameterTemplate, GLfloat emScale)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=9,
    error('invalid number of arguments');
end

r = moglcore( 'glPathMemoryGlyphIndexArrayNV', firstPathName, fontTarget, fontSize, fontData, faceIndex, firstGlyphIndex, numGlyphs, pathParameterTemplate, emScale );

return
