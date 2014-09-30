function image = glGetConvolutionFilterEXT( target, format, type )

% glGetConvolutionFilterEXT  Interface to OpenGL function glGetConvolutionFilterEXT
%
% usage:  image = glGetConvolutionFilterEXT( target, format, type )
%
% C function:  void glGetConvolutionFilterEXT(GLenum target, GLenum format, GLenum type, void* image)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

image = (0);

moglcore( 'glGetConvolutionFilterEXT', target, format, type, image );

return
