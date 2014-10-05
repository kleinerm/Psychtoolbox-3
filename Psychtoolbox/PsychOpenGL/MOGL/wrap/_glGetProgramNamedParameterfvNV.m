function params = glGetProgramNamedParameterfvNV( id, len, name )

% glGetProgramNamedParameterfvNV  Interface to OpenGL function glGetProgramNamedParameterfvNV
%
% usage:  params = glGetProgramNamedParameterfvNV( id, len, name )
%
% C function:  void glGetProgramNamedParameterfvNV(GLuint id, GLsizei len, const GLubyte* name, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetProgramNamedParameterfvNV', id, len, uint8(name), params );

return
