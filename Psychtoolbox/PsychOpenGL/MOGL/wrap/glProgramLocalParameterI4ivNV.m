function glProgramLocalParameterI4ivNV( target, index, params )

% glProgramLocalParameterI4ivNV  Interface to OpenGL function glProgramLocalParameterI4ivNV
%
% usage:  glProgramLocalParameterI4ivNV( target, index, params )
%
% C function:  void glProgramLocalParameterI4ivNV(GLenum target, GLuint index, const GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glProgramLocalParameterI4ivNV', target, index, int32(params) );

return
