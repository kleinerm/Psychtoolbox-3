function glVertexArrayEdgeFlagOffsetEXT( vaobj, buffer, stride, offset )

% glVertexArrayEdgeFlagOffsetEXT  Interface to OpenGL function glVertexArrayEdgeFlagOffsetEXT
%
% usage:  glVertexArrayEdgeFlagOffsetEXT( vaobj, buffer, stride, offset )
%
% C function:  void glVertexArrayEdgeFlagOffsetEXT(GLuint vaobj, GLuint buffer, GLsizei stride, GLintptr offset)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glVertexArrayEdgeFlagOffsetEXT', vaobj, buffer, stride, offset );

return
