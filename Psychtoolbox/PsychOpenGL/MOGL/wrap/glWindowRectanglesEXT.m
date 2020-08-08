function glWindowRectanglesEXT( mode, count, box )

% glWindowRectanglesEXT  Interface to OpenGL function glWindowRectanglesEXT
%
% usage:  glWindowRectanglesEXT( mode, count, box )
%
% C function:  void glWindowRectanglesEXT(GLenum mode, GLsizei count, const GLint* box)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glWindowRectanglesEXT', mode, count, int32(box) );

return
