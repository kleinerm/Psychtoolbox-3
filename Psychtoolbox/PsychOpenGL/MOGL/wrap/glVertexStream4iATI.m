function glVertexStream4iATI( stream, x, y, z, w )

% glVertexStream4iATI  Interface to OpenGL function glVertexStream4iATI
%
% usage:  glVertexStream4iATI( stream, x, y, z, w )
%
% C function:  void glVertexStream4iATI(GLenum stream, GLint x, GLint y, GLint z, GLint w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glVertexStream4iATI', stream, x, y, z, w );

return
