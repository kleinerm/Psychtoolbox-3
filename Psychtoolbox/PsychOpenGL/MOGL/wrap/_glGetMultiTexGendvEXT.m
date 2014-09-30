function params = glGetMultiTexGendvEXT( texunit, coord, pname )

% glGetMultiTexGendvEXT  Interface to OpenGL function glGetMultiTexGendvEXT
%
% usage:  params = glGetMultiTexGendvEXT( texunit, coord, pname )
%
% C function:  void glGetMultiTexGendvEXT(GLenum texunit, GLenum coord, GLenum pname, GLdouble* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = double(0);

moglcore( 'glGetMultiTexGendvEXT', texunit, coord, pname, params );

return
