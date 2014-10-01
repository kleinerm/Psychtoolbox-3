function params = glGetMinmaxParameterivEXT( target, pname )

% glGetMinmaxParameterivEXT  Interface to OpenGL function glGetMinmaxParameterivEXT
%
% usage:  params = glGetMinmaxParameterivEXT( target, pname )
%
% C function:  void glGetMinmaxParameterivEXT(GLenum target, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetMinmaxParameterivEXT', target, pname, params );

return
