function glVertexArrayVertexAttribIFormatEXT( vaobj, attribindex, size, type, relativeoffset )

% glVertexArrayVertexAttribIFormatEXT  Interface to OpenGL function glVertexArrayVertexAttribIFormatEXT
%
% usage:  glVertexArrayVertexAttribIFormatEXT( vaobj, attribindex, size, type, relativeoffset )
%
% C function:  void glVertexArrayVertexAttribIFormatEXT(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glVertexArrayVertexAttribIFormatEXT', vaobj, attribindex, size, type, relativeoffset );

return
