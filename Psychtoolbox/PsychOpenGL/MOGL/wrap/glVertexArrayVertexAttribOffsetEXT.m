function glVertexArrayVertexAttribOffsetEXT( vaobj, buffer, index, size, type, normalized, stride, offset )

% glVertexArrayVertexAttribOffsetEXT  Interface to OpenGL function glVertexArrayVertexAttribOffsetEXT
%
% usage:  glVertexArrayVertexAttribOffsetEXT( vaobj, buffer, index, size, type, normalized, stride, offset )
%
% C function:  void glVertexArrayVertexAttribOffsetEXT(GLuint vaobj, GLuint buffer, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLintptr offset)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=8,
    error('invalid number of arguments');
end

moglcore( 'glVertexArrayVertexAttribOffsetEXT', vaobj, buffer, index, size, type, normalized, stride, offset );

return
