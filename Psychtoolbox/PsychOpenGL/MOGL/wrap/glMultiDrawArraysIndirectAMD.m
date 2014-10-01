function glMultiDrawArraysIndirectAMD( mode, indirect, primcount, stride )

% glMultiDrawArraysIndirectAMD  Interface to OpenGL function glMultiDrawArraysIndirectAMD
%
% usage:  glMultiDrawArraysIndirectAMD( mode, indirect, primcount, stride )
%
% C function:  void glMultiDrawArraysIndirectAMD(GLenum mode, const void* indirect, GLsizei primcount, GLsizei stride)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glMultiDrawArraysIndirectAMD', mode, indirect, primcount, stride );

return
