function glUseProgram( program )

% glUseProgram  Interface to OpenGL function glUseProgram
%
% usage:  glUseProgram( program )
%
% C function:  void glUseProgram(GLuint program)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glUseProgram', program );

return
