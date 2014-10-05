function glVertexArrayVertexOffsetEXT( vaobj, buffer, size, type, stride, offset )

% glVertexArrayVertexOffsetEXT  Interface to OpenGL function glVertexArrayVertexOffsetEXT
%
% usage:  glVertexArrayVertexOffsetEXT( vaobj, buffer, size, type, stride, offset )
%
% C function:  void glVertexArrayVertexOffsetEXT(GLuint vaobj, GLuint buffer, GLint size, GLenum type, GLsizei stride, GLintptr offset)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glVertexArrayVertexOffsetEXT', vaobj, buffer, size, type, stride, offset );

return
