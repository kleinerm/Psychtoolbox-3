function glProgramParameteriEXT( program, pname, value )

% glProgramParameteriEXT  Interface to OpenGL function glProgramParameteriEXT
%
% usage:  glProgramParameteriEXT( program, pname, value )
%
% C function:  void glProgramParameteriEXT(GLuint program, GLenum pname, GLint value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glProgramParameteriEXT', program, pname, value );

return
