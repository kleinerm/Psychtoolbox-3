function r = glMapBufferRange( target, ptr1, ptr2, access )

% glMapBufferRange  Interface to OpenGL function glMapBufferRange
%
% usage:  r = glMapBufferRange( target, ptr1, ptr2, access )
%
% C function:  GLvoid* glMapBufferRange(GLenum target, GLint ptr, GLsizei ptr, GLbitfield access)

% 30-Aug-2012 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

r = moglcore( 'glMapBufferRange', target, ptr1, ptr2, access );

return
