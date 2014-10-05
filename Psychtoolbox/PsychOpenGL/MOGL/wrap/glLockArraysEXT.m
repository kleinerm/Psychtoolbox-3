function glLockArraysEXT( first, count )

% glLockArraysEXT  Interface to OpenGL function glLockArraysEXT
%
% usage:  glLockArraysEXT( first, count )
%
% C function:  void glLockArraysEXT(GLint first, GLsizei count)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glLockArraysEXT', first, count );

return
