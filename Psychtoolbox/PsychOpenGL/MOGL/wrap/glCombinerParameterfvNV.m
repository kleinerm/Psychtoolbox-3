function glCombinerParameterfvNV( pname, params )

% glCombinerParameterfvNV  Interface to OpenGL function glCombinerParameterfvNV
%
% usage:  glCombinerParameterfvNV( pname, params )
%
% C function:  void glCombinerParameterfvNV(GLenum pname, const GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glCombinerParameterfvNV', pname, single(params) );

return
