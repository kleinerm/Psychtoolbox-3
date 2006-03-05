function glBitmap( width, height, xorig, yorig, xmove, ymove, bitmap )

% glBitmap  Interface to OpenGL function glBitmap
%
% usage:  glBitmap( width, height, xorig, yorig, xmove, ymove, bitmap )
%
% C function:  void glBitmap(GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte* bitmap)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glBitmap', width, height, xorig, yorig, xmove, ymove, uint8(bitmap) );

return
