function glVertexArrayVertexAttribIOffsetEXT( vaobj, buffer, index, size, type, stride, offset )

% glVertexArrayVertexAttribIOffsetEXT  Interface to OpenGL function glVertexArrayVertexAttribIOffsetEXT
%
% usage:  glVertexArrayVertexAttribIOffsetEXT( vaobj, buffer, index, size, type, stride, offset )
%
% C function:  void glVertexArrayVertexAttribIOffsetEXT(GLuint vaobj, GLuint buffer, GLuint index, GLint size, GLenum type, GLsizei stride, GLintptr offset)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glVertexArrayVertexAttribIOffsetEXT', vaobj, buffer, index, size, type, stride, offset );

return
