function glFeedbackBuffer(  bufsize, buftype, bufferptr )
% glFeedbackBuffer  Interface to OpenGL function glFeedbackBuffer
%
% usage:  glFeedbackBuffer( bufsize, buftype, bufferptr )
%
% bufsize is the number of float values that can be written into the buffer.
% Caution: A float takes up 4 bytes, so the size of the allocated buffer needs
% to be at least 4*bufsize!
%
% buftype is a GLenum specifiying the type of requested information. See OpenGL manual.
%
% bufferptr is a pointer to a buffer allocated via bufferptr=moglmalloc() or bufferptr=moglcalloc().
%
% Example of usage:
%
% 1. Allocate a buffer of proper size to hold the information you want OpenGL to return:
%    bufferptr = moglmalloc(bufsize * 4);
%
% 2. Set this memory buffer as feedback buffer and tell GL which kind of info about each
%    primitive should be fed back. E.g., GL.2D would return projected 2D screen coordinates of
%    each transformed vertex.
%    glFeedbackBuffer(bufsize, GL.2D, bufferptr);
%
% 3. Switch OpenGL from normal rasterization (=drawing) mode into feedback mode. Executing
%    drawing commands will not create any visible drawing, but will only store the results
%    of geometric transform, culling and lighting operations into your buffer 'bufferptr':
%    glRenderMode(GL.FEEDBACK);
%
% 4. Draw your scene...
%
% 5. Switch back to normal rendering. This will return the number of actually fed back items:
%    itemcount = glRenderMode(GL.RENDER);
%
% 6. Return the content of the memory feedback buffer as a new Matlab matrix:
%    myfeedbackresult = moglgetbuffer(bufferptr, GL.FLOAT, itemcount*4);
%
% 7. Interpret and use content of myfeedbackresult as described in the manual for glFeedbackBuffer.

%   
% C function:  void glFeedbackBuffer(GLsizei size, GLenum type, GLfloat* buffer)

% 16-May-2006 -- created by MK.

% ---allocate---
% ---protected---

if nargin < 1 || isempty(bufsize) || bufsize <1
error('Missing or invalid bufsize argument.');
end;

if nargin < 2 || isempty(buftype)
error('Missing buftype argument.');
end;

if nargin < 3 || isempty(bufferptr)
error('Missing bufferptr argument.');
end;

moglcore( 'glFeedbackBuffer', bufsize, buftype, bufferptr );

return
