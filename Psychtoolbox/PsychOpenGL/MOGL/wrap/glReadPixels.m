function retpixels = glReadPixels( x, y, width, height, format, type )

% glReadPixels  Interface to OpenGL function glReadPixels
%
% usage:  retpixels = glReadPixels( x, y, width, height, format, type )
%
% C function:  void glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* retpixels)

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

if x < 0 | y<0
    error('Invalid (negative) (x,y) offset passed to glReadPixels.');
end;

if width <= 0 | height<=0
    error('Invalid (negative or zero) (width, height) passed to glReadPixels.');
end;

switch(format)
    case {GL.RED, GL.GREEN, GL.BLUE, GL.ALPHA, GL.LUMINANCE, GL.INTENSITY, GL.DEPTH_COMPONENT, GL.STENCIL_INDEX }
        numperpixel = 1;
    case {GL.LUMINANCE_ALPHA }
        numperpixel = 2;
    case {GL.RGB , GL.BGR }
        numperpixel = 3;
    case {GL.RGBA , GL.BGRA }
        numperpixel = 4;
    otherwise
        error('Invalid format passed to glReadPixels.');
end;    
        
switch(type)
    case {GL.UNSIGNED_BYTE , GL.BYTE }
        numsize = 1;
    case { GL.UNSIGNED_SHORT , GL.SHORT }
        numsize = 2;
    case { GL.UNSIGNED_INT , GL.INT , GL.FLOAT }
        numsize = 4;
    otherwise
        error('Invalid type passed to glReadPixels.');
end;

% Allocate memory:
pixels=zeros(numperpixel, width, height);

% Tell OpenGL that we accept byte-aligned aka unaligned data.
glPixelStorei(GL.PACK_ALIGNMENT, 1);

% Perform proper type-cast:
switch(type)
    case GL.UNSIGNED_BYTE
        pixels = uint8(pixels);
    case GL.BYTE
        pixels = int8(pixels);
    case GL.UNSIGNED_SHORT
        pixels = uint16(pixels);
    case GL.SHORT
        pixels = int16(pixels);
    case GL.UNSIGNED_INT
        pixels = uint32(pixels);
    case GL.INT
        pixels = int32(pixels);
    case GL.FLOAT
        pixels = moglsingle(pixels);
end;

% Execute actual call:
moglcore( 'glReadPixels', x, y, width, height, format, type, pixels );

% Rearrange data in Matlab friendly format:
for i=1:numperpixel
retpixels(:,:,i) = pixels(i,:,:);
end;

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
