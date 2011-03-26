function equation = glGetClipPlane( plane )

% glGetClipPlane  Interface to OpenGL function glGetClipPlane
%
% usage:  equation = glGetClipPlane( plane )
%
% C function:  void glGetClipPlane(GLenum plane, GLdouble* equation)

% 24-Jan-2006 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=1,
    error('invalid number of arguments');
end

equation = double(zeros(4,1));
moglcore( 'glGetClipPlane', plane, equation );

return
% ---skip---
