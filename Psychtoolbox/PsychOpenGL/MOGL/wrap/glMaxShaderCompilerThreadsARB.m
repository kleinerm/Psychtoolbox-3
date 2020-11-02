function glMaxShaderCompilerThreadsARB( count )

% glMaxShaderCompilerThreadsARB  Interface to OpenGL function glMaxShaderCompilerThreadsARB
%
% usage:  glMaxShaderCompilerThreadsARB( count )
%
% C function:  void glMaxShaderCompilerThreadsARB(GLuint count)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glMaxShaderCompilerThreadsARB', count );

return
