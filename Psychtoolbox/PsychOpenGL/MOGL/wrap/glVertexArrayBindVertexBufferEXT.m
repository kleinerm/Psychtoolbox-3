function glVertexArrayBindVertexBufferEXT( vaobj, bindingindex, buffer, offset, stride )

% glVertexArrayBindVertexBufferEXT  Interface to OpenGL function glVertexArrayBindVertexBufferEXT
%
% usage:  glVertexArrayBindVertexBufferEXT( vaobj, bindingindex, buffer, offset, stride )
%
% C function:  void glVertexArrayBindVertexBufferEXT(GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glVertexArrayBindVertexBufferEXT', vaobj, bindingindex, buffer, offset, stride );

return
