function glGetBufferSubData( target, offset, dsize, data )

% glGetBufferSubData  Interface to OpenGL function glGetBufferSubData
%
% usage:  glGetBufferSubData( target, offset, dsize, data )
%
% Caution: Caller needs to allocate 'data' as a matrix or vector of
% suitable format and capacity for the data, otherwise bad things will
% happen!
%
% C function:  void glGetBufferSubData(GLenum target, GLint offset, GLsizei dsize, GLvoid* data)

% 05-Mar-2006 -- created (generated automatically from header files)
% 03-Nov-2010 -- Made functional (MK).

% ---protected---

if nargin~=4,
    error('invalid number of arguments');
end

moglcore('glGetBufferSubData', target, offset, dsize, data);

return
% ---skip---
