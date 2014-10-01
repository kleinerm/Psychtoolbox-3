function glMapBufferARB( target, access )

% glMapBufferARB  Interface to OpenGL function glMapBufferARB
%
% usage:  glMapBufferARB( target, access )
%
% C function:  void* glMapBufferARB(GLenum target, GLenum access)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMapBufferARB', target, access );

return
