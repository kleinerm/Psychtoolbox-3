function params = glGetTexGendv( coord, pname )

% glGetTexGendv  Interface to OpenGL function glGetTexGendv
%
% usage:  params = glGetTexGendv( coord, pname )
%
% C function:  void glGetTexGendv(GLenum coord, GLenum pname, GLdouble* params)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = double(0);

moglcore( 'glGetTexGendv', coord, pname, params );

return
