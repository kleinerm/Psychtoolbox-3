function r = glImportSyncEXT( external_sync_type, external_sync, flags )

% glImportSyncEXT  Interface to OpenGL function glImportSyncEXT
%
% usage:  r = glImportSyncEXT( external_sync_type, external_sync, flags )
%
% C function:  GLsync glImportSyncEXT(GLenum external_sync_type, GLintptr external_sync, GLbitfield flags)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

r = moglcore( 'glImportSyncEXT', external_sync_type, external_sync, flags );

return
