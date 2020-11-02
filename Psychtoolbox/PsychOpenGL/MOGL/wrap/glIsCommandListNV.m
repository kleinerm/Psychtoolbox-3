function r = glIsCommandListNV( list )

% glIsCommandListNV  Interface to OpenGL function glIsCommandListNV
%
% usage:  r = glIsCommandListNV( list )
%
% C function:  GLboolean glIsCommandListNV(GLuint list)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glIsCommandListNV', list );

return
