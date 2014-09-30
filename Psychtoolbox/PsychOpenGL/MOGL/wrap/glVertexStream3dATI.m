function glVertexStream3dATI( stream, x, y, z )

% glVertexStream3dATI  Interface to OpenGL function glVertexStream3dATI
%
% usage:  glVertexStream3dATI( stream, x, y, z )
%
% C function:  void glVertexStream3dATI(GLenum stream, GLdouble x, GLdouble y, GLdouble z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glVertexStream3dATI', stream, x, y, z );

return
