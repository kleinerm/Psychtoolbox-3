function glColorTableParameterivSGI( target, pname, params )

% glColorTableParameterivSGI  Interface to OpenGL function glColorTableParameterivSGI
%
% usage:  glColorTableParameterivSGI( target, pname, params )
%
% C function:  void glColorTableParameterivSGI(GLenum target, GLenum pname, const GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glColorTableParameterivSGI', target, pname, int32(params) );

return
