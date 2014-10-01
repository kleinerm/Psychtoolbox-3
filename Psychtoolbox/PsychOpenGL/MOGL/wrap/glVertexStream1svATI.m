function glVertexStream1svATI( stream, coords )

% glVertexStream1svATI  Interface to OpenGL function glVertexStream1svATI
%
% usage:  glVertexStream1svATI( stream, coords )
%
% C function:  void glVertexStream1svATI(GLenum stream, const GLshort* coords)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexStream1svATI', stream, int16(coords) );

return
