function glSetFragmentShaderConstantATI( dst, value )

% glSetFragmentShaderConstantATI  Interface to OpenGL function glSetFragmentShaderConstantATI
%
% usage:  glSetFragmentShaderConstantATI( dst, value )
%
% C function:  void glSetFragmentShaderConstantATI(GLuint dst, const GLfloat* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glSetFragmentShaderConstantATI', dst, single(value) );

return
