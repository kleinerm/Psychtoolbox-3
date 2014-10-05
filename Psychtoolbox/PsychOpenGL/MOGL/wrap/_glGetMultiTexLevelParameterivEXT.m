function params = glGetMultiTexLevelParameterivEXT( texunit, target, level, pname )

% glGetMultiTexLevelParameterivEXT  Interface to OpenGL function glGetMultiTexLevelParameterivEXT
%
% usage:  params = glGetMultiTexLevelParameterivEXT( texunit, target, level, pname )
%
% C function:  void glGetMultiTexLevelParameterivEXT(GLenum texunit, GLenum target, GLint level, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=4,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetMultiTexLevelParameterivEXT', texunit, target, level, pname, params );

return
