function glFogCoorddvEXT( coord )

% glFogCoorddvEXT  Interface to OpenGL function glFogCoorddvEXT
%
% usage:  glFogCoorddvEXT( coord )
%
% C function:  void glFogCoorddvEXT(const GLdouble* coord)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glFogCoorddvEXT', double(coord) );

return
