function glVertexArrayElementBuffer( vaobj, buffer )

% glVertexArrayElementBuffer  Interface to OpenGL function glVertexArrayElementBuffer
%
% usage:  glVertexArrayElementBuffer( vaobj, buffer )
%
% C function:  void glVertexArrayElementBuffer(GLuint vaobj, GLuint buffer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexArrayElementBuffer', vaobj, buffer );

return
