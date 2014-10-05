function glFogCoordfvEXT( coord )

% glFogCoordfvEXT  Interface to OpenGL function glFogCoordfvEXT
%
% usage:  glFogCoordfvEXT( coord )
%
% C function:  void glFogCoordfvEXT(const GLfloat* coord)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glFogCoordfvEXT', single(coord) );

return
