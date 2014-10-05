function buffers = glCreateBuffers( n )

% glCreateBuffers  Interface to OpenGL function glCreateBuffers
%
% usage:  buffers = glCreateBuffers( n )
%
% C function:  void glCreateBuffers(GLsizei n, GLuint* buffers)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

buffers = uint32(0);

moglcore( 'glCreateBuffers', n, buffers );

return
