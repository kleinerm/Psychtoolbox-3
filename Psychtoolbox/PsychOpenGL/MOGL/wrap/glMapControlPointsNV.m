function glMapControlPointsNV( target, index, type, ustride, vstride, uorder, vorder, packed, points )

% glMapControlPointsNV  Interface to OpenGL function glMapControlPointsNV
%
% usage:  glMapControlPointsNV( target, index, type, ustride, vstride, uorder, vorder, packed, points )
%
% C function:  void glMapControlPointsNV(GLenum target, GLuint index, GLenum type, GLsizei ustride, GLsizei vstride, GLint uorder, GLint vorder, GLboolean packed, const void* points)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=9,
    error('invalid number of arguments');
end

moglcore( 'glMapControlPointsNV', target, index, type, ustride, vstride, uorder, vorder, packed, points );

return
