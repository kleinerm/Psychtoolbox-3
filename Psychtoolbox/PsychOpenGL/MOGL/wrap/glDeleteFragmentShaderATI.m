function glDeleteFragmentShaderATI( id )

% glDeleteFragmentShaderATI  Interface to OpenGL function glDeleteFragmentShaderATI
%
% usage:  glDeleteFragmentShaderATI( id )
%
% C function:  void glDeleteFragmentShaderATI(GLuint id)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glDeleteFragmentShaderATI', id );

return
