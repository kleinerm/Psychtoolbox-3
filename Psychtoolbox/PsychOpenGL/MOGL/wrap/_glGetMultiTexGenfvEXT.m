function params = glGetMultiTexGenfvEXT( texunit, coord, pname )

% glGetMultiTexGenfvEXT  Interface to OpenGL function glGetMultiTexGenfvEXT
%
% usage:  params = glGetMultiTexGenfvEXT( texunit, coord, pname )
%
% C function:  void glGetMultiTexGenfvEXT(GLenum texunit, GLenum coord, GLenum pname, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetMultiTexGenfvEXT', texunit, coord, pname, params );

return
