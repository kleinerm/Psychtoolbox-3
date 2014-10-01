function programs = glGenProgramsNV( n )

% glGenProgramsNV  Interface to OpenGL function glGenProgramsNV
%
% usage:  programs = glGenProgramsNV( n )
%
% C function:  void glGenProgramsNV(GLsizei n, GLuint* programs)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

programs = uint32(0);

moglcore( 'glGenProgramsNV', n, programs );

return
