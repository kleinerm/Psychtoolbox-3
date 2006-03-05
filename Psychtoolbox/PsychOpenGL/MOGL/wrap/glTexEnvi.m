function glTexEnvi( target, pname, param )

% glTexEnvi  Interface to OpenGL function glTexEnvi
%
% usage:  glTexEnvi( target, pname, param )
%
% C function:  void glTexEnvi(GLenum target, GLenum pname, GLint param)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTexEnvi', target, pname, param );

return
