function glCombinerParameterivNV( pname, params )

% glCombinerParameterivNV  Interface to OpenGL function glCombinerParameterivNV
%
% usage:  glCombinerParameterivNV( pname, params )
%
% C function:  void glCombinerParameterivNV(GLenum pname, const GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glCombinerParameterivNV', pname, int32(params) );

return
