function params = glGetProgramNamedParameterdvNV( id, len, name )

% glGetProgramNamedParameterdvNV  Interface to OpenGL function glGetProgramNamedParameterdvNV
%
% usage:  params = glGetProgramNamedParameterdvNV( id, len, name )
%
% C function:  void glGetProgramNamedParameterdvNV(GLuint id, GLsizei len, const GLubyte* name, GLdouble* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = double(0);

moglcore( 'glGetProgramNamedParameterdvNV', id, len, uint8(name), params );

return
