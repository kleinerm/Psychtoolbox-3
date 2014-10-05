function params = glGetProgramivNV( id, pname )

% glGetProgramivNV  Interface to OpenGL function glGetProgramivNV
%
% usage:  params = glGetProgramivNV( id, pname )
%
% C function:  void glGetProgramivNV(GLuint id, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetProgramivNV', id, pname, params );

return
