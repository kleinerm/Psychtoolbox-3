function glTexGeni( coord, pname, param )

% glTexGeni  Interface to OpenGL function glTexGeni
%
% usage:  glTexGeni( coord, pname, param )
%
% C function:  void glTexGeni(GLenum coord, GLenum pname, GLint param)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTexGeni', coord, pname, param );

return
