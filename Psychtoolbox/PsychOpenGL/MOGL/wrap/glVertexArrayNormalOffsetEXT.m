function glVertexArrayNormalOffsetEXT( vaobj, buffer, type, stride, offset )

% glVertexArrayNormalOffsetEXT  Interface to OpenGL function glVertexArrayNormalOffsetEXT
%
% usage:  glVertexArrayNormalOffsetEXT( vaobj, buffer, type, stride, offset )
%
% C function:  void glVertexArrayNormalOffsetEXT(GLuint vaobj, GLuint buffer, GLenum type, GLsizei stride, GLintptr offset)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glVertexArrayNormalOffsetEXT', vaobj, buffer, type, stride, offset );

return
