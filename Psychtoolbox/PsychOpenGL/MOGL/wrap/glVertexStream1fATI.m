function glVertexStream1fATI( stream, x )

% glVertexStream1fATI  Interface to OpenGL function glVertexStream1fATI
%
% usage:  glVertexStream1fATI( stream, x )
%
% C function:  void glVertexStream1fATI(GLenum stream, GLfloat x)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexStream1fATI', stream, x );

return
