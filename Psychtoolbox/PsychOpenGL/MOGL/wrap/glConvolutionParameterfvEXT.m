function glConvolutionParameterfvEXT( target, pname, params )

% glConvolutionParameterfvEXT  Interface to OpenGL function glConvolutionParameterfvEXT
%
% usage:  glConvolutionParameterfvEXT( target, pname, params )
%
% C function:  void glConvolutionParameterfvEXT(GLenum target, GLenum pname, const GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glConvolutionParameterfvEXT', target, pname, single(params) );

return
