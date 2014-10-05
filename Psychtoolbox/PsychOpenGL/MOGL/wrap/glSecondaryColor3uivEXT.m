function glSecondaryColor3uivEXT( v )

% glSecondaryColor3uivEXT  Interface to OpenGL function glSecondaryColor3uivEXT
%
% usage:  glSecondaryColor3uivEXT( v )
%
% C function:  void glSecondaryColor3uivEXT(const GLuint* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glSecondaryColor3uivEXT', uint32(v) );

return
