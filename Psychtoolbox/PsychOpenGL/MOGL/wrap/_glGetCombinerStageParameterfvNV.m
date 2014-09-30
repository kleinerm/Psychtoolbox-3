function params = glGetCombinerStageParameterfvNV( stage, pname )

% glGetCombinerStageParameterfvNV  Interface to OpenGL function glGetCombinerStageParameterfvNV
%
% usage:  params = glGetCombinerStageParameterfvNV( stage, pname )
%
% C function:  void glGetCombinerStageParameterfvNV(GLenum stage, GLenum pname, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetCombinerStageParameterfvNV', stage, pname, params );

return
