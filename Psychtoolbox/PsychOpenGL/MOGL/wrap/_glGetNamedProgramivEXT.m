function params = glGetNamedProgramivEXT( program, target, pname )

% glGetNamedProgramivEXT  Interface to OpenGL function glGetNamedProgramivEXT
%
% usage:  params = glGetNamedProgramivEXT( program, target, pname )
%
% C function:  void glGetNamedProgramivEXT(GLuint program, GLenum target, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetNamedProgramivEXT', program, target, pname, params );

return
