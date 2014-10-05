function glActiveVaryingNV( program, name )

% glActiveVaryingNV  Interface to OpenGL function glActiveVaryingNV
%
% usage:  glActiveVaryingNV( program, name )
%
% C function:  void glActiveVaryingNV(GLuint program, const GLchar* name)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glActiveVaryingNV', program, uint8(name) );

return
