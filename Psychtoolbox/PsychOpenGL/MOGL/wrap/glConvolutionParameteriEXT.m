function glConvolutionParameteriEXT( target, pname, params )

% glConvolutionParameteriEXT  Interface to OpenGL function glConvolutionParameteriEXT
%
% usage:  glConvolutionParameteriEXT( target, pname, params )
%
% C function:  void glConvolutionParameteriEXT(GLenum target, GLenum pname, GLint params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glConvolutionParameteriEXT', target, pname, params );

return
