function params = glGetMultiTexGenivEXT( texunit, coord, pname )

% glGetMultiTexGenivEXT  Interface to OpenGL function glGetMultiTexGenivEXT
%
% usage:  params = glGetMultiTexGenivEXT( texunit, coord, pname )
%
% C function:  void glGetMultiTexGenivEXT(GLenum texunit, GLenum coord, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetMultiTexGenivEXT', texunit, coord, pname, params );

return
