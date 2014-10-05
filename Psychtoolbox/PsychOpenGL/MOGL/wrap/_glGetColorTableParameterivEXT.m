function params = glGetColorTableParameterivEXT( target, pname )

% glGetColorTableParameterivEXT  Interface to OpenGL function glGetColorTableParameterivEXT
%
% usage:  params = glGetColorTableParameterivEXT( target, pname )
%
% C function:  void glGetColorTableParameterivEXT(GLenum target, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetColorTableParameterivEXT', target, pname, params );

return
