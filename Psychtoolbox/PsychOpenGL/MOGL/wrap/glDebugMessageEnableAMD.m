function glDebugMessageEnableAMD( category, severity, count, ids, enabled )

% glDebugMessageEnableAMD  Interface to OpenGL function glDebugMessageEnableAMD
%
% usage:  glDebugMessageEnableAMD( category, severity, count, ids, enabled )
%
% C function:  void glDebugMessageEnableAMD(GLenum category, GLenum severity, GLsizei count, const GLuint* ids, GLboolean enabled)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glDebugMessageEnableAMD', category, severity, count, uint32(ids), enabled );

return
