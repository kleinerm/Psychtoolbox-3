function glVertexStream1iATI( stream, x )

% glVertexStream1iATI  Interface to OpenGL function glVertexStream1iATI
%
% usage:  glVertexStream1iATI( stream, x )
%
% C function:  void glVertexStream1iATI(GLenum stream, GLint x)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexStream1iATI', stream, x );

return
