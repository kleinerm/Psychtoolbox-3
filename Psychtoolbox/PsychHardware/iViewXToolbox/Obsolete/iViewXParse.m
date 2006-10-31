function result=iViewXParse(data)

% parse data string for iView commands

result.command='ET_CHG';
result.param=1+round(rand*4);

