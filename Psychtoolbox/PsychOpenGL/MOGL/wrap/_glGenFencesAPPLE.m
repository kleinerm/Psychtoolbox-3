function fences = glGenFencesAPPLE( n )

% glGenFencesAPPLE  Interface to OpenGL function glGenFencesAPPLE
%
% usage:  fences = glGenFencesAPPLE( n )
%
% C function:  void glGenFencesAPPLE(GLsizei n, GLuint* fences)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

fences = uint32(0);

moglcore( 'glGenFencesAPPLE', n, fences );

return
