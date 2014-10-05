function glVertexStream4fvATI( stream, coords )

% glVertexStream4fvATI  Interface to OpenGL function glVertexStream4fvATI
%
% usage:  glVertexStream4fvATI( stream, coords )
%
% C function:  void glVertexStream4fvATI(GLenum stream, const GLfloat* coords)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexStream4fvATI', stream, single(coords) );

return
