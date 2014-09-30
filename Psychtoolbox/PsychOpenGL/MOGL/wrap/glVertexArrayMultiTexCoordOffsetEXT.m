function glVertexArrayMultiTexCoordOffsetEXT( vaobj, buffer, texunit, size, type, stride, offset )

% glVertexArrayMultiTexCoordOffsetEXT  Interface to OpenGL function glVertexArrayMultiTexCoordOffsetEXT
%
% usage:  glVertexArrayMultiTexCoordOffsetEXT( vaobj, buffer, texunit, size, type, stride, offset )
%
% C function:  void glVertexArrayMultiTexCoordOffsetEXT(GLuint vaobj, GLuint buffer, GLenum texunit, GLint size, GLenum type, GLsizei stride, GLintptr offset)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glVertexArrayMultiTexCoordOffsetEXT', vaobj, buffer, texunit, size, type, stride, offset );

return
