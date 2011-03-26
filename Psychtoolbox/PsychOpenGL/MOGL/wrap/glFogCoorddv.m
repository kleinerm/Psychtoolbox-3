function glFogCoorddv( coord )

% glFogCoorddv  Interface to OpenGL function glFogCoorddv
%
% usage:  glFogCoorddv( coord )
%
% C function:  void glFogCoorddv(const GLdouble* coord)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glFogCoorddv', double(coord) );

return
