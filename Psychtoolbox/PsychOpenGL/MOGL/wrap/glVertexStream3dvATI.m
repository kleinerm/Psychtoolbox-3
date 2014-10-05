function glVertexStream3dvATI( stream, coords )

% glVertexStream3dvATI  Interface to OpenGL function glVertexStream3dvATI
%
% usage:  glVertexStream3dvATI( stream, coords )
%
% C function:  void glVertexStream3dvATI(GLenum stream, const GLdouble* coords)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexStream3dvATI', stream, double(coords) );

return
