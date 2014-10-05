function params = glGetFinalCombinerInputParameterfvNV( variable, pname )

% glGetFinalCombinerInputParameterfvNV  Interface to OpenGL function glGetFinalCombinerInputParameterfvNV
%
% usage:  params = glGetFinalCombinerInputParameterfvNV( variable, pname )
%
% C function:  void glGetFinalCombinerInputParameterfvNV(GLenum variable, GLenum pname, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetFinalCombinerInputParameterfvNV', variable, pname, params );

return
