function glTexParameteri( target, pname, param )

% glTexParameteri  Interface to OpenGL function glTexParameteri
%
% usage:  glTexParameteri( target, pname, param )
%
% C function:  void glTexParameteri(GLenum target, GLenum pname, GLint param)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTexParameteri', target, pname, param );

return
