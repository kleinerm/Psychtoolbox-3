function glVertexStream3iATI( stream, x, y, z )

% glVertexStream3iATI  Interface to OpenGL function glVertexStream3iATI
%
% usage:  glVertexStream3iATI( stream, x, y, z )
%
% C function:  void glVertexStream3iATI(GLenum stream, GLint x, GLint y, GLint z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glVertexStream3iATI', stream, x, y, z );

return
