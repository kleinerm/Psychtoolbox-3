function params = glGetConvolutionParameterfvEXT( target, pname )

% glGetConvolutionParameterfvEXT  Interface to OpenGL function glGetConvolutionParameterfvEXT
%
% usage:  params = glGetConvolutionParameterfvEXT( target, pname )
%
% C function:  void glGetConvolutionParameterfvEXT(GLenum target, GLenum pname, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetConvolutionParameterfvEXT', target, pname, params );

return
