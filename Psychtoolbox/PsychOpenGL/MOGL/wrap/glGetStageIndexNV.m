function r = glGetStageIndexNV( shadertype )

% glGetStageIndexNV  Interface to OpenGL function glGetStageIndexNV
%
% usage:  r = glGetStageIndexNV( shadertype )
%
% C function:  GLushort glGetStageIndexNV(GLenum shadertype)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glGetStageIndexNV', shadertype );

return
