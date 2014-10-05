function glMultiDrawElementsIndirectAMD( mode, type, indirect, primcount, stride )

% glMultiDrawElementsIndirectAMD  Interface to OpenGL function glMultiDrawElementsIndirectAMD
%
% usage:  glMultiDrawElementsIndirectAMD( mode, type, indirect, primcount, stride )
%
% C function:  void glMultiDrawElementsIndirectAMD(GLenum mode, GLenum type, const void* indirect, GLsizei primcount, GLsizei stride)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glMultiDrawElementsIndirectAMD', mode, type, indirect, primcount, stride );

return
