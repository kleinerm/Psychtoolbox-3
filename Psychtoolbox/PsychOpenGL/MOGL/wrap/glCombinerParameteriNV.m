function glCombinerParameteriNV( pname, param )

% glCombinerParameteriNV  Interface to OpenGL function glCombinerParameteriNV
%
% usage:  glCombinerParameteriNV( pname, param )
%
% C function:  void glCombinerParameteriNV(GLenum pname, GLint param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glCombinerParameteriNV', pname, param );

return
