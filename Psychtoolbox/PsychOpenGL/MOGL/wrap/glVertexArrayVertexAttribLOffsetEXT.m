function glVertexArrayVertexAttribLOffsetEXT( vaobj, buffer, index, size, type, stride, offset )

% glVertexArrayVertexAttribLOffsetEXT  Interface to OpenGL function glVertexArrayVertexAttribLOffsetEXT
%
% usage:  glVertexArrayVertexAttribLOffsetEXT( vaobj, buffer, index, size, type, stride, offset )
%
% C function:  void glVertexArrayVertexAttribLOffsetEXT(GLuint vaobj, GLuint buffer, GLuint index, GLint size, GLenum type, GLsizei stride, GLintptr offset)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glVertexArrayVertexAttribLOffsetEXT', vaobj, buffer, index, size, type, stride, offset );

return
