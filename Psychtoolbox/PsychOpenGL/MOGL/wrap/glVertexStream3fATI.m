function glVertexStream3fATI( stream, x, y, z )

% glVertexStream3fATI  Interface to OpenGL function glVertexStream3fATI
%
% usage:  glVertexStream3fATI( stream, x, y, z )
%
% C function:  void glVertexStream3fATI(GLenum stream, GLfloat x, GLfloat y, GLfloat z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glVertexStream3fATI', stream, x, y, z );

return
