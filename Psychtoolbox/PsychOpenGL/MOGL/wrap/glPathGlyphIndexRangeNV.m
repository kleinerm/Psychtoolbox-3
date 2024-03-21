function [r, baseAndCount] = glPathGlyphIndexRangeNV( fontTarget, fontName, fontStyle, pathParameterTemplate, emScale, baseAndCount )

% glPathGlyphIndexRangeNV  Interface to OpenGL function glPathGlyphIndexRangeNV
%
% usage:  [r, baseAndCount] = glPathGlyphIndexRangeNV( fontTarget, fontName, fontStyle, pathParameterTemplate, emScale )
%
% C function:  GLenum glPathGlyphIndexRangeNV(GLenum fontTarget, const void* fontName, GLbitfield fontStyle, GLuint pathParameterTemplate, GLfloat emScale, GLuint baseAndCount[2])

% 08-Aug-2020 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=5
    error('invalid number of arguments');
end

baseAndCount = zeros(2, 1, 'uint32');
r = moglcore( 'glPathGlyphIndexRangeNV', fontTarget, fontName, fontStyle, pathParameterTemplate, emScale, baseAndCount );

return
