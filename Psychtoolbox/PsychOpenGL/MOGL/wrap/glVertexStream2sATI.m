function glVertexStream2sATI( stream, x, y )

% glVertexStream2sATI  Interface to OpenGL function glVertexStream2sATI
%
% usage:  glVertexStream2sATI( stream, x, y )
%
% C function:  void glVertexStream2sATI(GLenum stream, GLshort x, GLshort y)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexStream2sATI', stream, x, y );

return
