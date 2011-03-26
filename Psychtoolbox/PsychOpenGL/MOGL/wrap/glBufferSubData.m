function glBufferSubData( target, vbooffset, datasize, data )

% glBufferSubData  Interface to OpenGL function glBufferSubData
%
% usage:  glBufferSubData( target, vbooffset, datasize, data )
%
% C function:  void glBufferSubData(GLenum target, GLint vbooffset, GLsizei datasize, const GLvoid* data)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---protected---
% ---skip---

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glBufferSubData', target, vbooffset, datasize, data );

return
