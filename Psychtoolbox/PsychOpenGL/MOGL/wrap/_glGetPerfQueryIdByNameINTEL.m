function [ queryName, queryId ] = glGetPerfQueryIdByNameINTEL

% glGetPerfQueryIdByNameINTEL  Interface to OpenGL function glGetPerfQueryIdByNameINTEL
%
% usage:  [ queryName, queryId ] = glGetPerfQueryIdByNameINTEL
%
% C function:  void glGetPerfQueryIdByNameINTEL(GLchar* queryName, GLuint* queryId)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=0,
    error('invalid number of arguments');
end

queryName = uint8(0);
queryId = uint32(0);

moglcore( 'glGetPerfQueryIdByNameINTEL', queryName, queryId );

return
