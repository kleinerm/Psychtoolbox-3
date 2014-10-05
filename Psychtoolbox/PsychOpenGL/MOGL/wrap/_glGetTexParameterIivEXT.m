function params = glGetTexParameterIivEXT( target, pname )

% glGetTexParameterIivEXT  Interface to OpenGL function glGetTexParameterIivEXT
%
% usage:  params = glGetTexParameterIivEXT( target, pname )
%
% C function:  void glGetTexParameterIivEXT(GLenum target, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetTexParameterIivEXT', target, pname, params );

return
