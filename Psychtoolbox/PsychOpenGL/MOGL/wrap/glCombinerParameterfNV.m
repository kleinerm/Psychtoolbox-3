function glCombinerParameterfNV( pname, param )

% glCombinerParameterfNV  Interface to OpenGL function glCombinerParameterfNV
%
% usage:  glCombinerParameterfNV( pname, param )
%
% C function:  void glCombinerParameterfNV(GLenum pname, GLfloat param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glCombinerParameterfNV', pname, param );

return
