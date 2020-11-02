function r = glPathGlyphIndexRangeNV( fontTarget, fontName, fontStyle, pathParameterTemplate, emScale, baseAndCount )

% glPathGlyphIndexRangeNV  Interface to OpenGL function glPathGlyphIndexRangeNV
%
% usage:  r = glPathGlyphIndexRangeNV( fontTarget, fontName, fontStyle, pathParameterTemplate, emScale, baseAndCount )
%
% C function:  GLenum glPathGlyphIndexRangeNV(GLenum fontTarget, const void* fontName, GLbitfield fontStyle, GLuint pathParameterTemplate, GLfloat emScale, GLuint baseAndCount)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

r = moglcore( 'glPathGlyphIndexRangeNV', fontTarget, fontName, fontStyle, pathParameterTemplate, emScale, baseAndCount );

return
