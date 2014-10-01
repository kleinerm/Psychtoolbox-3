function glVertexStream4svATI( stream, coords )

% glVertexStream4svATI  Interface to OpenGL function glVertexStream4svATI
%
% usage:  glVertexStream4svATI( stream, coords )
%
% C function:  void glVertexStream4svATI(GLenum stream, const GLshort* coords)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexStream4svATI', stream, int16(coords) );

return
