function glStencilThenCoverFillPathNV( path, fillMode, mask, coverMode )

% glStencilThenCoverFillPathNV  Interface to OpenGL function glStencilThenCoverFillPathNV
%
% usage:  glStencilThenCoverFillPathNV( path, fillMode, mask, coverMode )
%
% C function:  void glStencilThenCoverFillPathNV(GLuint path, GLenum fillMode, GLuint mask, GLenum coverMode)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glStencilThenCoverFillPathNV', path, fillMode, mask, coverMode );

return
