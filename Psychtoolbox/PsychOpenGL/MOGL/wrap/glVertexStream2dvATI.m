function glVertexStream2dvATI( stream, coords )

% glVertexStream2dvATI  Interface to OpenGL function glVertexStream2dvATI
%
% usage:  glVertexStream2dvATI( stream, coords )
%
% C function:  void glVertexStream2dvATI(GLenum stream, const GLdouble* coords)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexStream2dvATI', stream, double(coords) );

return
