function glVertexArrayVertexAttribFormatEXT( vaobj, attribindex, size, type, normalized, relativeoffset )

% glVertexArrayVertexAttribFormatEXT  Interface to OpenGL function glVertexArrayVertexAttribFormatEXT
%
% usage:  glVertexArrayVertexAttribFormatEXT( vaobj, attribindex, size, type, normalized, relativeoffset )
%
% C function:  void glVertexArrayVertexAttribFormatEXT(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glVertexArrayVertexAttribFormatEXT', vaobj, attribindex, size, type, normalized, relativeoffset );

return
