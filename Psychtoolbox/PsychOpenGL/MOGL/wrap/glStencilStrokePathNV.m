function glStencilStrokePathNV( path, reference, mask )

% glStencilStrokePathNV  Interface to OpenGL function glStencilStrokePathNV
%
% usage:  glStencilStrokePathNV( path, reference, mask )
%
% C function:  void glStencilStrokePathNV(GLuint path, GLint reference, GLuint mask)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glStencilStrokePathNV', path, reference, mask );

return
