function glSecondaryColor3svEXT( v )

% glSecondaryColor3svEXT  Interface to OpenGL function glSecondaryColor3svEXT
%
% usage:  glSecondaryColor3svEXT( v )
%
% C function:  void glSecondaryColor3svEXT(const GLshort* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glSecondaryColor3svEXT', int16(v) );

return
