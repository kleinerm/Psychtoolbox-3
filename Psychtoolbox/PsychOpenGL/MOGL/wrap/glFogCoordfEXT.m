function glFogCoordfEXT( coord )

% glFogCoordfEXT  Interface to OpenGL function glFogCoordfEXT
%
% usage:  glFogCoordfEXT( coord )
%
% C function:  void glFogCoordfEXT(GLfloat coord)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glFogCoordfEXT', coord );

return
