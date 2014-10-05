function glVertexStream3fvATI( stream, coords )

% glVertexStream3fvATI  Interface to OpenGL function glVertexStream3fvATI
%
% usage:  glVertexStream3fvATI( stream, coords )
%
% C function:  void glVertexStream3fvATI(GLenum stream, const GLfloat* coords)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexStream3fvATI', stream, single(coords) );

return
