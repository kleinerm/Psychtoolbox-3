function glActiveProgramEXT( program )

% glActiveProgramEXT  Interface to OpenGL function glActiveProgramEXT
%
% usage:  glActiveProgramEXT( program )
%
% C function:  void glActiveProgramEXT(GLuint program)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glActiveProgramEXT', program );

return
