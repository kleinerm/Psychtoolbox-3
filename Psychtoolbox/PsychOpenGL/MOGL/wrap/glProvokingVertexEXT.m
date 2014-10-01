function glProvokingVertexEXT( mode )

% glProvokingVertexEXT  Interface to OpenGL function glProvokingVertexEXT
%
% usage:  glProvokingVertexEXT( mode )
%
% C function:  void glProvokingVertexEXT(GLenum mode)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glProvokingVertexEXT', mode );

return
