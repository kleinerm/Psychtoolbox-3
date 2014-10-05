function glVertexStream4ivATI( stream, coords )

% glVertexStream4ivATI  Interface to OpenGL function glVertexStream4ivATI
%
% usage:  glVertexStream4ivATI( stream, coords )
%
% C function:  void glVertexStream4ivATI(GLenum stream, const GLint* coords)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexStream4ivATI', stream, int32(coords) );

return
