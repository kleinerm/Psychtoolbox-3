function glSecondaryColor3usvEXT( v )

% glSecondaryColor3usvEXT  Interface to OpenGL function glSecondaryColor3usvEXT
%
% usage:  glSecondaryColor3usvEXT( v )
%
% C function:  void glSecondaryColor3usvEXT(const GLushort* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glSecondaryColor3usvEXT', uint16(v) );

return
