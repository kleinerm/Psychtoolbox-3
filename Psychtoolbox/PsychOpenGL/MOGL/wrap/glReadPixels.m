function pixels = glReadPixels( x, y, width, height, format, type )

% glReadPixels  Interface to OpenGL function glReadPixels
%
% usage:  pixels = glReadPixels( x, y, width, height, format, type )
%
% C function:  void glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels)

% 24-Jan-2006 -- created (generated automatically from header files)
% 25-Jan-2006 -- return argument allocated for case format=GL_RGB,
%                type=GL_UNSIGNED_BYTE (RFM)

% ---allocate---
% ---protected---

if nargin~=6,
    error('invalid number of arguments');
end

% check format and type
global GL
if format~=GL.RGB || type~=GL.UNSIGNED_BYTE,
    error('glReadPixels only implemented for format=GL_RGB, type=GL_UNSIGNED_BYTE');
end

% rows occupy an even multiple of 'align' bytes
% ( should look up value in OpenGL state;  assume four for now )
align=4;

% each number takes 'numsize' bytes
% ( should deduce value from 'type' argument;  assume GL_UNSIGNED_BYTE for now )
numsize=1;

% there are 'numperpixel' numbers for each pixel
% ( should deduce value from 'format' argument;  assume GL_RGB for now )
numperpixel=3;

% allocate return argument
padwidth=align*ceil(numsize*numperpixel*width/align);
n=padwidth*height;
pixels=uint8(zeros(n,1));

moglcore( 'glReadPixels', x, y, width, height, format, type, pixels );

return


% ---autocode---
%
% function pixels = glReadPixels( x, y, width, height, format, type )
% 
% % glReadPixels  Interface to OpenGL function glReadPixels
% %
% % usage:  pixels = glReadPixels( x, y, width, height, format, type )
% %
% % C function:  void glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels)
% 
% % 05-Mar-2006 -- created (generated automatically from header files)
% 
% % ---allocate---
% 
% if nargin~=6,
%     error('invalid number of arguments');
% end
% 
% pixels = (0);
% 
% moglcore( 'glReadPixels', x, y, width, height, format, type, pixels );
% 
% return
%
