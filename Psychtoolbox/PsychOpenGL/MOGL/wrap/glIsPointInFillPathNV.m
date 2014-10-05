function r = glIsPointInFillPathNV( path, mask, x, y )

% glIsPointInFillPathNV  Interface to OpenGL function glIsPointInFillPathNV
%
% usage:  r = glIsPointInFillPathNV( path, mask, x, y )
%
% C function:  GLboolean glIsPointInFillPathNV(GLuint path, GLuint mask, GLfloat x, GLfloat y)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

r = moglcore( 'glIsPointInFillPathNV', path, mask, x, y );

return
