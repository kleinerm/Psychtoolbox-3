function glFogCoorddv( coord )

% glFogCoorddv  Interface to OpenGL function glFogCoorddv
%
% usage:  glFogCoorddv( coord )
%
% C function:  void glFogCoorddv(const GLdouble* coord)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glFogCoorddv', double(coord) );

return
