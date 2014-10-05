function glVertexStream2dATI( stream, x, y )

% glVertexStream2dATI  Interface to OpenGL function glVertexStream2dATI
%
% usage:  glVertexStream2dATI( stream, x, y )
%
% C function:  void glVertexStream2dATI(GLenum stream, GLdouble x, GLdouble y)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexStream2dATI', stream, x, y );

return
