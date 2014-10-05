function glVertexArrayAttribFormat( vaobj, attribindex, size, type, normalized, relativeoffset )

% glVertexArrayAttribFormat  Interface to OpenGL function glVertexArrayAttribFormat
%
% usage:  glVertexArrayAttribFormat( vaobj, attribindex, size, type, normalized, relativeoffset )
%
% C function:  void glVertexArrayAttribFormat(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glVertexArrayAttribFormat', vaobj, attribindex, size, type, normalized, relativeoffset );

return
