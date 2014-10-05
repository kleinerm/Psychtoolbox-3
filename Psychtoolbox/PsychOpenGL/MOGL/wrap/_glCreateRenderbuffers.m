function renderbuffers = glCreateRenderbuffers( n )

% glCreateRenderbuffers  Interface to OpenGL function glCreateRenderbuffers
%
% usage:  renderbuffers = glCreateRenderbuffers( n )
%
% C function:  void glCreateRenderbuffers(GLsizei n, GLuint* renderbuffers)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

renderbuffers = uint32(0);

moglcore( 'glCreateRenderbuffers', n, renderbuffers );

return
