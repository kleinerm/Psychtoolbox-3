function r = glIsList( list )

% glIsList  Interface to OpenGL function glIsList
%
% usage:  r = glIsList( list )
%
% C function:  GLboolean glIsList(GLuint list)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glIsList', list );

return
