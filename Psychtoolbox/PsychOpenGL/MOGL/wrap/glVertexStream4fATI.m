function glVertexStream4fATI( stream, x, y, z, w )

% glVertexStream4fATI  Interface to OpenGL function glVertexStream4fATI
%
% usage:  glVertexStream4fATI( stream, x, y, z, w )
%
% C function:  void glVertexStream4fATI(GLenum stream, GLfloat x, GLfloat y, GLfloat z, GLfloat w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glVertexStream4fATI', stream, x, y, z, w );

return
