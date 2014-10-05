function params = glGetConvolutionParameterivEXT( target, pname )

% glGetConvolutionParameterivEXT  Interface to OpenGL function glGetConvolutionParameterivEXT
%
% usage:  params = glGetConvolutionParameterivEXT( target, pname )
%
% C function:  void glGetConvolutionParameterivEXT(GLenum target, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetConvolutionParameterivEXT', target, pname, params );

return
