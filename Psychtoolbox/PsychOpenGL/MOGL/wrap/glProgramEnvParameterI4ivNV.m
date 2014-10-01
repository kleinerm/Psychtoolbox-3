function glProgramEnvParameterI4ivNV( target, index, params )

% glProgramEnvParameterI4ivNV  Interface to OpenGL function glProgramEnvParameterI4ivNV
%
% usage:  glProgramEnvParameterI4ivNV( target, index, params )
%
% C function:  void glProgramEnvParameterI4ivNV(GLenum target, GLuint index, const GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glProgramEnvParameterI4ivNV', target, index, int32(params) );

return
