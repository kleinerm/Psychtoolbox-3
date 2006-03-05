function buffer = glSelectBuffer( size )

% glSelectBuffer  Interface to OpenGL function glSelectBuffer
%
% usage:  buffer = glSelectBuffer( size )
%
% C function:  void glSelectBuffer(GLsizei size, GLuint* buffer)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

buffer = uint32(0);

moglcore( 'glSelectBuffer', size, buffer );

return
