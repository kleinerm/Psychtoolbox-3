function glCoverStrokePathNV( path, coverMode )

% glCoverStrokePathNV  Interface to OpenGL function glCoverStrokePathNV
%
% usage:  glCoverStrokePathNV( path, coverMode )
%
% C function:  void glCoverStrokePathNV(GLuint path, GLenum coverMode)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glCoverStrokePathNV', path, coverMode );

return
