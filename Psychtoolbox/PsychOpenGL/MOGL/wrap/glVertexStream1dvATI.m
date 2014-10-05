function glVertexStream1dvATI( stream, coords )

% glVertexStream1dvATI  Interface to OpenGL function glVertexStream1dvATI
%
% usage:  glVertexStream1dvATI( stream, coords )
%
% C function:  void glVertexStream1dvATI(GLenum stream, const GLdouble* coords)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexStream1dvATI', stream, double(coords) );

return
