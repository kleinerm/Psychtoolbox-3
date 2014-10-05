function glCopyImageSubDataNV( srcName, srcTarget, srcLevel, srcX, srcY, srcZ, dstName, dstTarget, dstLevel, dstX, dstY, dstZ, width, height, depth )

% glCopyImageSubDataNV  Interface to OpenGL function glCopyImageSubDataNV
%
% usage:  glCopyImageSubDataNV( srcName, srcTarget, srcLevel, srcX, srcY, srcZ, dstName, dstTarget, dstLevel, dstX, dstY, dstZ, width, height, depth )
%
% C function:  void glCopyImageSubDataNV(GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei width, GLsizei height, GLsizei depth)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=15,
    error('invalid number of arguments');
end

moglcore( 'glCopyImageSubDataNV', srcName, srcTarget, srcLevel, srcX, srcY, srcZ, dstName, dstTarget, dstLevel, dstX, dstY, dstZ, width, height, depth );

return
