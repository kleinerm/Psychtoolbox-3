function glVertexStream1dATI( stream, x )

% glVertexStream1dATI  Interface to OpenGL function glVertexStream1dATI
%
% usage:  glVertexStream1dATI( stream, x )
%
% C function:  void glVertexStream1dATI(GLenum stream, GLdouble x)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexStream1dATI', stream, x );

return
