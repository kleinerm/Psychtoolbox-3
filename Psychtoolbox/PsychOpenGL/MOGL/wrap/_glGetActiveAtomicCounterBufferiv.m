function params = glGetActiveAtomicCounterBufferiv( program, bufferIndex, pname )

% glGetActiveAtomicCounterBufferiv  Interface to OpenGL function glGetActiveAtomicCounterBufferiv
%
% usage:  params = glGetActiveAtomicCounterBufferiv( program, bufferIndex, pname )
%
% C function:  void glGetActiveAtomicCounterBufferiv(GLuint program, GLuint bufferIndex, GLenum pname, GLint* params)

% 30-Aug-2012 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=3,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetActiveAtomicCounterBufferiv', program, bufferIndex, pname, params );

return
