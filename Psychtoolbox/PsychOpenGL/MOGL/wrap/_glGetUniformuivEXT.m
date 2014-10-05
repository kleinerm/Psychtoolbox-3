function params = glGetUniformuivEXT( program, location )

% glGetUniformuivEXT  Interface to OpenGL function glGetUniformuivEXT
%
% usage:  params = glGetUniformuivEXT( program, location )
%
% C function:  void glGetUniformuivEXT(GLuint program, GLint location, GLuint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = uint32(0);

moglcore( 'glGetUniformuivEXT', program, location, params );

return
