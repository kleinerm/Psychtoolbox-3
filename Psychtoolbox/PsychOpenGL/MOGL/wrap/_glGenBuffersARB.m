function buffers = glGenBuffersARB( n )

% glGenBuffersARB  Interface to OpenGL function glGenBuffersARB
%
% usage:  buffers = glGenBuffersARB( n )
%
% C function:  void glGenBuffersARB(GLsizei n, GLuint* buffers)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

buffers = uint32(0);

moglcore( 'glGenBuffersARB', n, buffers );

return
