function glSecondaryColor3ivEXT( v )

% glSecondaryColor3ivEXT  Interface to OpenGL function glSecondaryColor3ivEXT
%
% usage:  glSecondaryColor3ivEXT( v )
%
% C function:  void glSecondaryColor3ivEXT(const GLint* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glSecondaryColor3ivEXT', int32(v) );

return
