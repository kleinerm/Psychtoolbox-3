function [ queryName, dataSize, noCounters, noInstances, capsMask ] = glGetPerfQueryInfoINTEL( queryId, queryNameLength )

% glGetPerfQueryInfoINTEL  Interface to OpenGL function glGetPerfQueryInfoINTEL
%
% usage:  [ queryName, dataSize, noCounters, noInstances, capsMask ] = glGetPerfQueryInfoINTEL( queryId, queryNameLength )
%
% C function:  void glGetPerfQueryInfoINTEL(GLuint queryId, GLuint queryNameLength, GLchar* queryName, GLuint* dataSize, GLuint* noCounters, GLuint* noInstances, GLuint* capsMask)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

queryName = uint8(0);
dataSize = uint32(0);
noCounters = uint32(0);
noInstances = uint32(0);
capsMask = uint32(0);

moglcore( 'glGetPerfQueryInfoINTEL', queryId, queryNameLength, queryName, dataSize, noCounters, noInstances, capsMask );

return
