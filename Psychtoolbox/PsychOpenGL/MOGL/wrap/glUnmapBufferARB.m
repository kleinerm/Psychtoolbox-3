function r = glUnmapBufferARB( target )

% glUnmapBufferARB  Interface to OpenGL function glUnmapBufferARB
%
% usage:  r = glUnmapBufferARB( target )
%
% C function:  GLboolean glUnmapBufferARB(GLenum target)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glUnmapBufferARB', target );

return
