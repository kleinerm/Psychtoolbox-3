function glDeleteShader( shader )

% glDeleteShader  Interface to OpenGL function glDeleteShader
%
% usage:  glDeleteShader( shader )
%
% C function:  void glDeleteShader(GLuint shader)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glDeleteShader', shader );

return
