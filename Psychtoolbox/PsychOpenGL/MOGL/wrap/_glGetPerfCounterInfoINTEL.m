function [ counterName, counterDesc, counterOffset, counterDataSize, counterTypeEnum, counterDataTypeEnum, rawCounterMaxValue ] = glGetPerfCounterInfoINTEL( queryId, counterId, counterNameLength, counterDescLength )

% glGetPerfCounterInfoINTEL  Interface to OpenGL function glGetPerfCounterInfoINTEL
%
% usage:  [ counterName, counterDesc, counterOffset, counterDataSize, counterTypeEnum, counterDataTypeEnum, rawCounterMaxValue ] = glGetPerfCounterInfoINTEL( queryId, counterId, counterNameLength, counterDescLength )
%
% C function:  void glGetPerfCounterInfoINTEL(GLuint queryId, GLuint counterId, GLuint counterNameLength, GLchar* counterName, GLuint counterDescLength, GLchar* counterDesc, GLuint* counterOffset, GLuint* counterDataSize, GLuint* counterTypeEnum, GLuint* counterDataTypeEnum, GLuint64* rawCounterMaxValue)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=4,
    error('invalid number of arguments');
end

counterName = uint8(0);
counterDesc = uint8(0);
counterOffset = uint32(0);
counterDataSize = uint32(0);
counterTypeEnum = uint32(0);
counterDataTypeEnum = uint32(0);
rawCounterMaxValue = uint64(0);

moglcore( 'glGetPerfCounterInfoINTEL', queryId, counterId, counterNameLength, counterName, counterDescLength, counterDesc, counterOffset, counterDataSize, counterTypeEnum, counterDataTypeEnum, rawCounterMaxValue );

return
