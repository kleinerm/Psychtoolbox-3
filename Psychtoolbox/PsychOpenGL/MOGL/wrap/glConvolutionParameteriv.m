function glConvolutionParameteriv( target, pname, params )

% glConvolutionParameteriv  Interface to OpenGL function glConvolutionParameteriv
%
% usage:  glConvolutionParameteriv( target, pname, params )
%
% C function:  void glConvolutionParameteriv(GLenum target, GLenum pname, const GLint* params)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glConvolutionParameteriv', target, pname, int32(params) );

return
