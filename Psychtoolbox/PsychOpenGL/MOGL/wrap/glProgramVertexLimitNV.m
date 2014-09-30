function glProgramVertexLimitNV( target, limit )

% glProgramVertexLimitNV  Interface to OpenGL function glProgramVertexLimitNV
%
% usage:  glProgramVertexLimitNV( target, limit )
%
% C function:  void glProgramVertexLimitNV(GLenum target, GLint limit)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glProgramVertexLimitNV', target, limit );

return
