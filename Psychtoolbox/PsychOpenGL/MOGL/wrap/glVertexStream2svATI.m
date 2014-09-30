function glVertexStream2svATI( stream, coords )

% glVertexStream2svATI  Interface to OpenGL function glVertexStream2svATI
%
% usage:  glVertexStream2svATI( stream, coords )
%
% C function:  void glVertexStream2svATI(GLenum stream, const GLshort* coords)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexStream2svATI', stream, int16(coords) );

return
