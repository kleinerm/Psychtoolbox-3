function params = glGetMultiTexParameterivEXT( texunit, target, pname )

% glGetMultiTexParameterivEXT  Interface to OpenGL function glGetMultiTexParameterivEXT
%
% usage:  params = glGetMultiTexParameterivEXT( texunit, target, pname )
%
% C function:  void glGetMultiTexParameterivEXT(GLenum texunit, GLenum target, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetMultiTexParameterivEXT', texunit, target, pname, params );

return
