function glProgramEnvParametersI4ivNV( target, index, count, params )

% glProgramEnvParametersI4ivNV  Interface to OpenGL function glProgramEnvParametersI4ivNV
%
% usage:  glProgramEnvParametersI4ivNV( target, index, count, params )
%
% C function:  void glProgramEnvParametersI4ivNV(GLenum target, GLuint index, GLsizei count, const GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramEnvParametersI4ivNV', target, index, count, int32(params) );

return
