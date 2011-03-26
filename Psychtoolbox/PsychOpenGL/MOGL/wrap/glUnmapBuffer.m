function r = glUnmapBuffer( target )

% glUnmapBuffer  Interface to OpenGL function glUnmapBuffer
%
% usage:  r = glUnmapBuffer( target )
%
% C function:  GLboolean glUnmapBuffer(GLenum target)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glUnmapBuffer', target );

return
