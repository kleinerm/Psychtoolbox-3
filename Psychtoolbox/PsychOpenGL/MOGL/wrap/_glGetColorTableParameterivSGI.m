function params = glGetColorTableParameterivSGI( target, pname )

% glGetColorTableParameterivSGI  Interface to OpenGL function glGetColorTableParameterivSGI
%
% usage:  params = glGetColorTableParameterivSGI( target, pname )
%
% C function:  void glGetColorTableParameterivSGI(GLenum target, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetColorTableParameterivSGI', target, pname, params );

return
