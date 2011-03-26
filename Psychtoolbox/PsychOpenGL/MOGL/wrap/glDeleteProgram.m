function glDeleteProgram( program )

% glDeleteProgram  Interface to OpenGL function glDeleteProgram
%
% usage:  glDeleteProgram( program )
%
% C function:  void glDeleteProgram(GLuint program)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glDeleteProgram', program );

return
