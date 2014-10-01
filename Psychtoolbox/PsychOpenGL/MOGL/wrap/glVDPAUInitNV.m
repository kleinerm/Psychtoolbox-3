function glVDPAUInitNV( vdpDevice, getProcAddress )

% glVDPAUInitNV  Interface to OpenGL function glVDPAUInitNV
%
% usage:  glVDPAUInitNV( vdpDevice, getProcAddress )
%
% C function:  void glVDPAUInitNV(const void* vdpDevice, const void* getProcAddress)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVDPAUInitNV', vdpDevice, getProcAddress );

return
