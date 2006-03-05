function buffer = glFeedbackBuffer( size, type )

% glFeedbackBuffer  Interface to OpenGL function glFeedbackBuffer
%
% usage:  buffer = glFeedbackBuffer( size, type )
%
% C function:  void glFeedbackBuffer(GLsizei size, GLenum type, GLfloat* buffer)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

buffer = single(0);

moglcore( 'glFeedbackBuffer', size, type, buffer );

return
