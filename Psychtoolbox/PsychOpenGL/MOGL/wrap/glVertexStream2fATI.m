function glVertexStream2fATI( stream, x, y )

% glVertexStream2fATI  Interface to OpenGL function glVertexStream2fATI
%
% usage:  glVertexStream2fATI( stream, x, y )
%
% C function:  void glVertexStream2fATI(GLenum stream, GLfloat x, GLfloat y)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexStream2fATI', stream, x, y );

return
