function glAttachShader( program, shader )

% glAttachShader  Interface to OpenGL function glAttachShader
%
% usage:  glAttachShader( program, shader )
%
% C function:  void glAttachShader(GLuint program, GLuint shader)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glAttachShader', program, shader );

return
