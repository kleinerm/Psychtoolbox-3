function glVertexStream1ivATI( stream, coords )

% glVertexStream1ivATI  Interface to OpenGL function glVertexStream1ivATI
%
% usage:  glVertexStream1ivATI( stream, coords )
%
% C function:  void glVertexStream1ivATI(GLenum stream, const GLint* coords)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexStream1ivATI', stream, int32(coords) );

return
