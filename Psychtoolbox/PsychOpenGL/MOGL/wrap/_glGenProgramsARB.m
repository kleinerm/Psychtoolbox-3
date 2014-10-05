function programs = glGenProgramsARB( n )

% glGenProgramsARB  Interface to OpenGL function glGenProgramsARB
%
% usage:  programs = glGenProgramsARB( n )
%
% C function:  void glGenProgramsARB(GLsizei n, GLuint* programs)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

programs = uint32(0);

moglcore( 'glGenProgramsARB', n, programs );

return
