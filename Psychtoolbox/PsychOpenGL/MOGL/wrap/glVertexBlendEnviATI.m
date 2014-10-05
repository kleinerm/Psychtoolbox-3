function glVertexBlendEnviATI( pname, param )

% glVertexBlendEnviATI  Interface to OpenGL function glVertexBlendEnviATI
%
% usage:  glVertexBlendEnviATI( pname, param )
%
% C function:  void glVertexBlendEnviATI(GLenum pname, GLint param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexBlendEnviATI', pname, param );

return
