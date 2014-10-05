function params = glGetProgramParameterfvNV( target, index, pname )

% glGetProgramParameterfvNV  Interface to OpenGL function glGetProgramParameterfvNV
%
% usage:  params = glGetProgramParameterfvNV( target, index, pname )
%
% C function:  void glGetProgramParameterfvNV(GLenum target, GLuint index, GLenum pname, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetProgramParameterfvNV', target, index, pname, params );

return
