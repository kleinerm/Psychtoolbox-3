function glTexGend( coord, pname, param )

% glTexGend  Interface to OpenGL function glTexGend
%
% usage:  glTexGend( coord, pname, param )
%
% C function:  void glTexGend(GLenum coord, GLenum pname, GLdouble param)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTexGend', coord, pname, param );

return
