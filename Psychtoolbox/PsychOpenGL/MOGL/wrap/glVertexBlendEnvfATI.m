function glVertexBlendEnvfATI( pname, param )

% glVertexBlendEnvfATI  Interface to OpenGL function glVertexBlendEnvfATI
%
% usage:  glVertexBlendEnvfATI( pname, param )
%
% C function:  void glVertexBlendEnvfATI(GLenum pname, GLfloat param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexBlendEnvfATI', pname, param );

return
