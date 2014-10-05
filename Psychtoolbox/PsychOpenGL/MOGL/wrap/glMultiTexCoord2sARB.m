function glMultiTexCoord2sARB( target, s, t )

% glMultiTexCoord2sARB  Interface to OpenGL function glMultiTexCoord2sARB
%
% usage:  glMultiTexCoord2sARB( target, s, t )
%
% C function:  void glMultiTexCoord2sARB(GLenum target, GLshort s, GLshort t)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord2sARB', target, s, t );

return
