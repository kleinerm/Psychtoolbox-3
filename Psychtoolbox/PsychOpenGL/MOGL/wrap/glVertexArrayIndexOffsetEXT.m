function glVertexArrayIndexOffsetEXT( vaobj, buffer, type, stride, offset )

% glVertexArrayIndexOffsetEXT  Interface to OpenGL function glVertexArrayIndexOffsetEXT
%
% usage:  glVertexArrayIndexOffsetEXT( vaobj, buffer, type, stride, offset )
%
% C function:  void glVertexArrayIndexOffsetEXT(GLuint vaobj, GLuint buffer, GLenum type, GLsizei stride, GLintptr offset)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glVertexArrayIndexOffsetEXT', vaobj, buffer, type, stride, offset );

return
