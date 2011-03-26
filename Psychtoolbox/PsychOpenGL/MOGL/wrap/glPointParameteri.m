function glPointParameteri( pname, param )

% glPointParameteri  Interface to OpenGL function glPointParameteri
%
% usage:  glPointParameteri( pname, param )
%
% C function:  void glPointParameteri(GLenum pname, GLint param)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glPointParameteri', pname, param );

return
