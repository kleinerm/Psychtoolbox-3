function glConvolutionParameterivEXT( target, pname, params )

% glConvolutionParameterivEXT  Interface to OpenGL function glConvolutionParameterivEXT
%
% usage:  glConvolutionParameterivEXT( target, pname, params )
%
% C function:  void glConvolutionParameterivEXT(GLenum target, GLenum pname, const GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glConvolutionParameterivEXT', target, pname, int32(params) );

return
