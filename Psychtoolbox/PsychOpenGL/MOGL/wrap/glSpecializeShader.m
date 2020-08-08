function glSpecializeShader( shader, pEntryPoint, numSpecializationConstants, pConstantIndex, pConstantValue )

% glSpecializeShader  Interface to OpenGL function glSpecializeShader
%
% usage:  glSpecializeShader( shader, pEntryPoint, numSpecializationConstants, pConstantIndex, pConstantValue )
%
% C function:  void glSpecializeShader(GLuint shader, const GLchar* pEntryPoint, GLuint numSpecializationConstants, const GLuint* pConstantIndex, const GLuint* pConstantValue)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glSpecializeShader', shader, uint8(pEntryPoint), numSpecializationConstants, uint32(pConstantIndex), uint32(pConstantValue) );

return
