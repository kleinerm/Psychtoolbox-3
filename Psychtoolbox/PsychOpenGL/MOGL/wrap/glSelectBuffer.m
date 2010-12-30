function glSelectBuffer( bufsize, bufferptr )
% glSelectBuffer  Interface to OpenGL function glSelectBuffer
%
% usage:  glSelectBuffer( bufsize, bufferptr )
%
%
% bufsize = The number of unsigned int values that can be placed into the buffer.
%
% bufferptr = Pointer to a buffer created via bufferptr=moglmalloc() or
%             bufferptr=moglcalloc().
%
%
% For principle of usage, see help glFeedbackBuffer. This buffer is meant
% to be used when OpenGL is switched into glRenderMode(GL.SELECT).
%
% C function:  void glSelectBuffer(GLsizei bufsize, GLuint* bufferptr)

% 16-May-2006 -- created by MK.

% ---allocate---
% ---protected---

if nargin < 1 || isempty(bufsize) || bufsize <1
error('Missing or invalid bufsize argument.');
end;

if nargin < 3 || isempty(bufferptr)
error('Missing bufferptr argument.');
end;

moglcore( 'glSelectBuffer', bufsize, bufferptr );

return
