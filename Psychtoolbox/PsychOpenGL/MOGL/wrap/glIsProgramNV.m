function r = glIsProgramNV( id )

% glIsProgramNV  Interface to OpenGL function glIsProgramNV
%
% usage:  r = glIsProgramNV( id )
%
% C function:  GLboolean glIsProgramNV(GLuint id)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glIsProgramNV', id );

return
