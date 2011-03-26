function glDeleteShader( shader )

% glDeleteShader  Interface to OpenGL function glDeleteShader
%
% usage:  glDeleteShader( shader )
%
% C function:  void glDeleteShader(GLuint shader)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glDeleteShader', shader );

return
