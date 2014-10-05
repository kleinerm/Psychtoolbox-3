function glSecondaryColor3bvEXT( v )

% glSecondaryColor3bvEXT  Interface to OpenGL function glSecondaryColor3bvEXT
%
% usage:  glSecondaryColor3bvEXT( v )
%
% C function:  void glSecondaryColor3bvEXT(const GLbyte* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glSecondaryColor3bvEXT', int8(v) );

return
