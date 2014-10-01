function points = glGetMapControlPointsNV( target, index, type, ustride, vstride, packed )

% glGetMapControlPointsNV  Interface to OpenGL function glGetMapControlPointsNV
%
% usage:  points = glGetMapControlPointsNV( target, index, type, ustride, vstride, packed )
%
% C function:  void glGetMapControlPointsNV(GLenum target, GLuint index, GLenum type, GLsizei ustride, GLsizei vstride, GLboolean packed, void* points)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=6,
    error('invalid number of arguments');
end

points = (0);

moglcore( 'glGetMapControlPointsNV', target, index, type, ustride, vstride, packed, points );

return
