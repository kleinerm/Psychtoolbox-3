function glPointSize( size )

% glPointSize  Interface to OpenGL function glPointSize
%
% usage:  glPointSize( size )
%
% C function:  void glPointSize(GLfloat size)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glPointSize', size );

return
