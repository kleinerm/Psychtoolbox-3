function r = glMapBuffer( target, access )

% glMapBuffer  Interface to OpenGL function glMapBuffer
%
% usage:  r = glMapBuffer( target, access )
%
% C function:  GLvoid* glMapBuffer(GLenum target, GLenum access)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

r = moglcore( 'glMapBuffer', target, access );

return
