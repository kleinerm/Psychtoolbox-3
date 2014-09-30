function glDeleteProgramsNV( n, programs )

% glDeleteProgramsNV  Interface to OpenGL function glDeleteProgramsNV
%
% usage:  glDeleteProgramsNV( n, programs )
%
% C function:  void glDeleteProgramsNV(GLsizei n, const GLuint* programs)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDeleteProgramsNV', n, uint32(programs) );

return
