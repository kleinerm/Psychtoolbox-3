function glVertexStream3svATI( stream, coords )

% glVertexStream3svATI  Interface to OpenGL function glVertexStream3svATI
%
% usage:  glVertexStream3svATI( stream, coords )
%
% C function:  void glVertexStream3svATI(GLenum stream, const GLshort* coords)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexStream3svATI', stream, int16(coords) );

return
