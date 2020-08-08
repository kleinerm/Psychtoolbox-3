function glStencilThenCoverStrokePathNV( path, reference, mask, coverMode )

% glStencilThenCoverStrokePathNV  Interface to OpenGL function glStencilThenCoverStrokePathNV
%
% usage:  glStencilThenCoverStrokePathNV( path, reference, mask, coverMode )
%
% C function:  void glStencilThenCoverStrokePathNV(GLuint path, GLint reference, GLuint mask, GLenum coverMode)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glStencilThenCoverStrokePathNV', path, reference, mask, coverMode );

return
