function params = glGetCombinerOutputParameterfvNV( stage, portion, pname )

% glGetCombinerOutputParameterfvNV  Interface to OpenGL function glGetCombinerOutputParameterfvNV
%
% usage:  params = glGetCombinerOutputParameterfvNV( stage, portion, pname )
%
% C function:  void glGetCombinerOutputParameterfvNV(GLenum stage, GLenum portion, GLenum pname, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetCombinerOutputParameterfvNV', stage, portion, pname, params );

return
