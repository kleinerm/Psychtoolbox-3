function glPointSize( size )

% glPointSize  Interface to OpenGL function glPointSize
%
% usage:  glPointSize( size )
%
% C function:  void glPointSize(GLfloat size)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glPointSize', size );

return
