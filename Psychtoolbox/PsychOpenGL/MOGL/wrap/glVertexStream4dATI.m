function glVertexStream4dATI( stream, x, y, z, w )

% glVertexStream4dATI  Interface to OpenGL function glVertexStream4dATI
%
% usage:  glVertexStream4dATI( stream, x, y, z, w )
%
% C function:  void glVertexStream4dATI(GLenum stream, GLdouble x, GLdouble y, GLdouble z, GLdouble w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glVertexStream4dATI', stream, x, y, z, w );

return
