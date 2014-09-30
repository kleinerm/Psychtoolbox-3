function glVertexStream1fvATI( stream, coords )

% glVertexStream1fvATI  Interface to OpenGL function glVertexStream1fvATI
%
% usage:  glVertexStream1fvATI( stream, coords )
%
% C function:  void glVertexStream1fvATI(GLenum stream, const GLfloat* coords)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexStream1fvATI', stream, single(coords) );

return
