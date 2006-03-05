function equation = glGetClipPlane( plane )

% glGetClipPlane  Interface to OpenGL function glGetClipPlane
%
% usage:  equation = glGetClipPlane( plane )
%
% C function:  void glGetClipPlane(GLenum plane, GLdouble* equation)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

equation = double(0);

moglcore( 'glGetClipPlane', plane, equation );

return
