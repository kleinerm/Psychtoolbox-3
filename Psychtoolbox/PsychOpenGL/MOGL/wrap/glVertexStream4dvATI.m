function glVertexStream4dvATI( stream, coords )

% glVertexStream4dvATI  Interface to OpenGL function glVertexStream4dvATI
%
% usage:  glVertexStream4dvATI( stream, coords )
%
% C function:  void glVertexStream4dvATI(GLenum stream, const GLdouble* coords)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexStream4dvATI', stream, double(coords) );

return
