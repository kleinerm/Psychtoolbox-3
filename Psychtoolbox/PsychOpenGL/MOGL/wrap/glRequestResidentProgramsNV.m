function glRequestResidentProgramsNV( n, programs )

% glRequestResidentProgramsNV  Interface to OpenGL function glRequestResidentProgramsNV
%
% usage:  glRequestResidentProgramsNV( n, programs )
%
% C function:  void glRequestResidentProgramsNV(GLsizei n, const GLuint* programs)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glRequestResidentProgramsNV', n, uint32(programs) );

return
