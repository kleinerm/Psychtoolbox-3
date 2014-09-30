function params = glGetCombinerInputParameterfvNV( stage, portion, variable, pname )

% glGetCombinerInputParameterfvNV  Interface to OpenGL function glGetCombinerInputParameterfvNV
%
% usage:  params = glGetCombinerInputParameterfvNV( stage, portion, variable, pname )
%
% C function:  void glGetCombinerInputParameterfvNV(GLenum stage, GLenum portion, GLenum variable, GLenum pname, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=4,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetCombinerInputParameterfvNV', stage, portion, variable, pname, params );

return
