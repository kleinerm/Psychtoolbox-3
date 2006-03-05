function glClipPlane( plane, equation )

% glClipPlane  Interface to OpenGL function glClipPlane
%
% usage:  glClipPlane( plane, equation )
%
% C function:  void glClipPlane(GLenum plane, const GLdouble* equation)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glClipPlane', plane, double(equation) );

return
