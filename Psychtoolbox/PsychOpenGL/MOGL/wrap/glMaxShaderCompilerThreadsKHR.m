function glMaxShaderCompilerThreadsKHR( count )

% glMaxShaderCompilerThreadsKHR  Interface to OpenGL function glMaxShaderCompilerThreadsKHR
%
% usage:  glMaxShaderCompilerThreadsKHR( count )
%
% C function:  void glMaxShaderCompilerThreadsKHR(GLuint count)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glMaxShaderCompilerThreadsKHR', count );

return
