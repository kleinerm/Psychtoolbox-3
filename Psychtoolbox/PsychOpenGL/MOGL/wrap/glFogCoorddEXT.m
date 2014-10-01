function glFogCoorddEXT( coord )

% glFogCoorddEXT  Interface to OpenGL function glFogCoorddEXT
%
% usage:  glFogCoorddEXT( coord )
%
% C function:  void glFogCoorddEXT(GLdouble coord)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glFogCoorddEXT', coord );

return
