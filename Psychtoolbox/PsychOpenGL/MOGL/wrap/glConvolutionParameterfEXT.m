function glConvolutionParameterfEXT( target, pname, params )

% glConvolutionParameterfEXT  Interface to OpenGL function glConvolutionParameterfEXT
%
% usage:  glConvolutionParameterfEXT( target, pname, params )
%
% C function:  void glConvolutionParameterfEXT(GLenum target, GLenum pname, GLfloat params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glConvolutionParameterfEXT', target, pname, params );

return
