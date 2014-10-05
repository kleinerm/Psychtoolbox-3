function glArrayElementEXT( i )

% glArrayElementEXT  Interface to OpenGL function glArrayElementEXT
%
% usage:  glArrayElementEXT( i )
%
% C function:  void glArrayElementEXT(GLint i)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glArrayElementEXT', i );

return
