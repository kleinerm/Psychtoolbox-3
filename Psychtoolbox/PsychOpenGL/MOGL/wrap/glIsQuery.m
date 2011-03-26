function r = glIsQuery( id )

% glIsQuery  Interface to OpenGL function glIsQuery
%
% usage:  r = glIsQuery( id )
%
% C function:  GLboolean glIsQuery(GLuint id)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glIsQuery', id );

return
