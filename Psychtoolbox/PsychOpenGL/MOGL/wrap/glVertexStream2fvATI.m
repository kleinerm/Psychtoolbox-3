function glVertexStream2fvATI( stream, coords )

% glVertexStream2fvATI  Interface to OpenGL function glVertexStream2fvATI
%
% usage:  glVertexStream2fvATI( stream, coords )
%
% C function:  void glVertexStream2fvATI(GLenum stream, const GLfloat* coords)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexStream2fvATI', stream, single(coords) );

return
