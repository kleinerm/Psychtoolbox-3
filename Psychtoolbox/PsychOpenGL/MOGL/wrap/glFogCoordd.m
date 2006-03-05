function glFogCoordd( coord )

% glFogCoordd  Interface to OpenGL function glFogCoordd
%
% usage:  glFogCoordd( coord )
%
% C function:  void glFogCoordd(GLdouble coord)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glFogCoordd', coord );

return
