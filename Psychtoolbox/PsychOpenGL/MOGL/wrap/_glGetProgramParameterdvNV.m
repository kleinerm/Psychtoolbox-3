function params = glGetProgramParameterdvNV( target, index, pname )

% glGetProgramParameterdvNV  Interface to OpenGL function glGetProgramParameterdvNV
%
% usage:  params = glGetProgramParameterdvNV( target, index, pname )
%
% C function:  void glGetProgramParameterdvNV(GLenum target, GLuint index, GLenum pname, GLdouble* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = double(0);

moglcore( 'glGetProgramParameterdvNV', target, index, pname, params );

return
