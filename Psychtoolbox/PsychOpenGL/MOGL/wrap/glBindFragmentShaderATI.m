function glBindFragmentShaderATI( id )

% glBindFragmentShaderATI  Interface to OpenGL function glBindFragmentShaderATI
%
% usage:  glBindFragmentShaderATI( id )
%
% C function:  void glBindFragmentShaderATI(GLuint id)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glBindFragmentShaderATI', id );

return
