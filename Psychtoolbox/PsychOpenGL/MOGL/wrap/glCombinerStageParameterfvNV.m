function glCombinerStageParameterfvNV( stage, pname, params )

% glCombinerStageParameterfvNV  Interface to OpenGL function glCombinerStageParameterfvNV
%
% usage:  glCombinerStageParameterfvNV( stage, pname, params )
%
% C function:  void glCombinerStageParameterfvNV(GLenum stage, GLenum pname, const GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glCombinerStageParameterfvNV', stage, pname, single(params) );

return
