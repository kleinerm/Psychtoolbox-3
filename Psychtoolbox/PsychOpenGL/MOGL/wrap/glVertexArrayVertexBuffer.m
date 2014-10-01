function glVertexArrayVertexBuffer( vaobj, bindingindex, buffer, offset, stride )

% glVertexArrayVertexBuffer  Interface to OpenGL function glVertexArrayVertexBuffer
%
% usage:  glVertexArrayVertexBuffer( vaobj, bindingindex, buffer, offset, stride )
%
% C function:  void glVertexArrayVertexBuffer(GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glVertexArrayVertexBuffer', vaobj, bindingindex, buffer, offset, stride );

return
