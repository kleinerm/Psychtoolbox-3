function fences = glGenFencesNV( n )

% glGenFencesNV  Interface to OpenGL function glGenFencesNV
%
% usage:  fences = glGenFencesNV( n )
%
% C function:  void glGenFencesNV(GLsizei n, GLuint* fences)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

fences = uint32(0);

moglcore( 'glGenFencesNV', n, fences );

return
