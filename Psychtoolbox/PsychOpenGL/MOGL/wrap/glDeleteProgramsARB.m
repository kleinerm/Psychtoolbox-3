function glDeleteProgramsARB( n, programs )

% glDeleteProgramsARB  Interface to OpenGL function glDeleteProgramsARB
%
% usage:  glDeleteProgramsARB( n, programs )
%
% C function:  void glDeleteProgramsARB(GLsizei n, const GLuint* programs)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDeleteProgramsARB', n, uint32(programs) );

return
