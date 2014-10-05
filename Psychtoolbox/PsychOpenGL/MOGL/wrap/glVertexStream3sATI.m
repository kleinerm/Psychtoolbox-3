function glVertexStream3sATI( stream, x, y, z )

% glVertexStream3sATI  Interface to OpenGL function glVertexStream3sATI
%
% usage:  glVertexStream3sATI( stream, x, y, z )
%
% C function:  void glVertexStream3sATI(GLenum stream, GLshort x, GLshort y, GLshort z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glVertexStream3sATI', stream, x, y, z );

return
