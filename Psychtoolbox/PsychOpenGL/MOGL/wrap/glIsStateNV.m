function r = glIsStateNV( state )

% glIsStateNV  Interface to OpenGL function glIsStateNV
%
% usage:  r = glIsStateNV( state )
%
% C function:  GLboolean glIsStateNV(GLuint state)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glIsStateNV', state );

return
