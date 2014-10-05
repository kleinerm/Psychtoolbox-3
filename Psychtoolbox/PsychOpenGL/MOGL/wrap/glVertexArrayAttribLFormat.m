function glVertexArrayAttribLFormat( vaobj, attribindex, size, type, relativeoffset )

% glVertexArrayAttribLFormat  Interface to OpenGL function glVertexArrayAttribLFormat
%
% usage:  glVertexArrayAttribLFormat( vaobj, attribindex, size, type, relativeoffset )
%
% C function:  void glVertexArrayAttribLFormat(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glVertexArrayAttribLFormat', vaobj, attribindex, size, type, relativeoffset );

return
