function r = glIsProgram( program )

% glIsProgram  Interface to OpenGL function glIsProgram
%
% usage:  r = glIsProgram( program )
%
% C function:  GLboolean glIsProgram(GLuint program)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glIsProgram', program );

return
