function glProgramPathFragmentInputGenNV( program, location, genMode, components, coeffs )

% glProgramPathFragmentInputGenNV  Interface to OpenGL function glProgramPathFragmentInputGenNV
%
% usage:  glProgramPathFragmentInputGenNV( program, location, genMode, components, coeffs )
%
% C function:  void glProgramPathFragmentInputGenNV(GLuint program, GLint location, GLenum genMode, GLint components, const GLfloat* coeffs)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glProgramPathFragmentInputGenNV', program, location, genMode, components, single(coeffs) );

return
