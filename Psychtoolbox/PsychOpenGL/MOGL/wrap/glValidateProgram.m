function glValidateProgram( program )

% glValidateProgram  Interface to OpenGL function glValidateProgram
%
% usage:  glValidateProgram( program )
%
% C function:  void glValidateProgram(GLuint program)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glValidateProgram', program );

return
