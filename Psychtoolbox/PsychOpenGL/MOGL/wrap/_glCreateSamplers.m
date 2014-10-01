function samplers = glCreateSamplers( n )

% glCreateSamplers  Interface to OpenGL function glCreateSamplers
%
% usage:  samplers = glCreateSamplers( n )
%
% C function:  void glCreateSamplers(GLsizei n, GLuint* samplers)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

samplers = uint32(0);

moglcore( 'glCreateSamplers', n, samplers );

return
