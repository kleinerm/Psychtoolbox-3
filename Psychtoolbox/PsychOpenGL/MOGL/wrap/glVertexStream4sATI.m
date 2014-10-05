function glVertexStream4sATI( stream, x, y, z, w )

% glVertexStream4sATI  Interface to OpenGL function glVertexStream4sATI
%
% usage:  glVertexStream4sATI( stream, x, y, z, w )
%
% C function:  void glVertexStream4sATI(GLenum stream, GLshort x, GLshort y, GLshort z, GLshort w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glVertexStream4sATI', stream, x, y, z, w );

return
