function glVertexArrayAttribIFormat( vaobj, attribindex, size, type, relativeoffset )

% glVertexArrayAttribIFormat  Interface to OpenGL function glVertexArrayAttribIFormat
%
% usage:  glVertexArrayAttribIFormat( vaobj, attribindex, size, type, relativeoffset )
%
% C function:  void glVertexArrayAttribIFormat(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glVertexArrayAttribIFormat', vaobj, attribindex, size, type, relativeoffset );

return
