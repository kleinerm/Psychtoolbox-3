function glVertexStream2ivATI( stream, coords )

% glVertexStream2ivATI  Interface to OpenGL function glVertexStream2ivATI
%
% usage:  glVertexStream2ivATI( stream, coords )
%
% C function:  void glVertexStream2ivATI(GLenum stream, const GLint* coords)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexStream2ivATI', stream, int32(coords) );

return
