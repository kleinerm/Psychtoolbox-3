function r = glIsBufferResidentNV( target )

% glIsBufferResidentNV  Interface to OpenGL function glIsBufferResidentNV
%
% usage:  r = glIsBufferResidentNV( target )
%
% C function:  GLboolean glIsBufferResidentNV(GLenum target)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glIsBufferResidentNV', target );

return
