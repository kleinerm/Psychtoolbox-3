function image = glGetConvolutionFilter( target, format, type )

% glGetConvolutionFilter  Interface to OpenGL function glGetConvolutionFilter
%
% usage:  image = glGetConvolutionFilter( target, format, type )
%
% C function:  void glGetConvolutionFilter(GLenum target, GLenum format, GLenum type, GLvoid* image)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

image = (0);

moglcore( 'glGetConvolutionFilter', target, format, type, image );

return
