function glCoverFillPathNV( path, coverMode )

% glCoverFillPathNV  Interface to OpenGL function glCoverFillPathNV
%
% usage:  glCoverFillPathNV( path, coverMode )
%
% C function:  void glCoverFillPathNV(GLuint path, GLenum coverMode)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glCoverFillPathNV', path, coverMode );

return
