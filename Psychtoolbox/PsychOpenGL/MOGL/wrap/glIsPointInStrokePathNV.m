function r = glIsPointInStrokePathNV( path, x, y )

% glIsPointInStrokePathNV  Interface to OpenGL function glIsPointInStrokePathNV
%
% usage:  r = glIsPointInStrokePathNV( path, x, y )
%
% C function:  GLboolean glIsPointInStrokePathNV(GLuint path, GLfloat x, GLfloat y)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

r = moglcore( 'glIsPointInStrokePathNV', path, x, y );

return
