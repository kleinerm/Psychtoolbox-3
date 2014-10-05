function glVertexArrayTexCoordOffsetEXT( vaobj, buffer, size, type, stride, offset )

% glVertexArrayTexCoordOffsetEXT  Interface to OpenGL function glVertexArrayTexCoordOffsetEXT
%
% usage:  glVertexArrayTexCoordOffsetEXT( vaobj, buffer, size, type, stride, offset )
%
% C function:  void glVertexArrayTexCoordOffsetEXT(GLuint vaobj, GLuint buffer, GLint size, GLenum type, GLsizei stride, GLintptr offset)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glVertexArrayTexCoordOffsetEXT', vaobj, buffer, size, type, stride, offset );

return
