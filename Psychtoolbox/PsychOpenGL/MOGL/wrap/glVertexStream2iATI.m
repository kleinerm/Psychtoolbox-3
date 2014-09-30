function glVertexStream2iATI( stream, x, y )

% glVertexStream2iATI  Interface to OpenGL function glVertexStream2iATI
%
% usage:  glVertexStream2iATI( stream, x, y )
%
% C function:  void glVertexStream2iATI(GLenum stream, GLint x, GLint y)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexStream2iATI', stream, x, y );

return
