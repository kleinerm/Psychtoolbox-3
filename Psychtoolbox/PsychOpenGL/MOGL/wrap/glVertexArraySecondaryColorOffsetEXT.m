function glVertexArraySecondaryColorOffsetEXT( vaobj, buffer, size, type, stride, offset )

% glVertexArraySecondaryColorOffsetEXT  Interface to OpenGL function glVertexArraySecondaryColorOffsetEXT
%
% usage:  glVertexArraySecondaryColorOffsetEXT( vaobj, buffer, size, type, stride, offset )
%
% C function:  void glVertexArraySecondaryColorOffsetEXT(GLuint vaobj, GLuint buffer, GLint size, GLenum type, GLsizei stride, GLintptr offset)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glVertexArraySecondaryColorOffsetEXT', vaobj, buffer, size, type, stride, offset );

return
