function glVertexStream1sATI( stream, x )

% glVertexStream1sATI  Interface to OpenGL function glVertexStream1sATI
%
% usage:  glVertexStream1sATI( stream, x )
%
% C function:  void glVertexStream1sATI(GLenum stream, GLshort x)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexStream1sATI', stream, x );

return
