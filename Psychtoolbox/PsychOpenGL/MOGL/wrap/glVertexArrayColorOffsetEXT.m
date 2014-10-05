function glVertexArrayColorOffsetEXT( vaobj, buffer, size, type, stride, offset )

% glVertexArrayColorOffsetEXT  Interface to OpenGL function glVertexArrayColorOffsetEXT
%
% usage:  glVertexArrayColorOffsetEXT( vaobj, buffer, size, type, stride, offset )
%
% C function:  void glVertexArrayColorOffsetEXT(GLuint vaobj, GLuint buffer, GLint size, GLenum type, GLsizei stride, GLintptr offset)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glVertexArrayColorOffsetEXT', vaobj, buffer, size, type, stride, offset );

return
