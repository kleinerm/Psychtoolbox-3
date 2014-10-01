function glVertexArrayVertexAttribLFormatEXT( vaobj, attribindex, size, type, relativeoffset )

% glVertexArrayVertexAttribLFormatEXT  Interface to OpenGL function glVertexArrayVertexAttribLFormatEXT
%
% usage:  glVertexArrayVertexAttribLFormatEXT( vaobj, attribindex, size, type, relativeoffset )
%
% C function:  void glVertexArrayVertexAttribLFormatEXT(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glVertexArrayVertexAttribLFormatEXT', vaobj, attribindex, size, type, relativeoffset );

return
