function glTexParameterf( target, pname, param )

% glTexParameterf  Interface to OpenGL function glTexParameterf
%
% usage:  glTexParameterf( target, pname, param )
%
% C function:  void glTexParameterf(GLenum target, GLenum pname, GLfloat param)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTexParameterf', target, pname, param );

return
