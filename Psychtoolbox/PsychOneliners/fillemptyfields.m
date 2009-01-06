function in = fillemptyfields(in,filling)
% in = fillemptyfields(in,filling)
%
% If input is a struct (-array):
% Fills all empty fields in IN with FILLING
% Also operates on N-D struct arrays
%
% If input is a cell (-array):
% Fills all empty elements in IN with FILLING
%
% DN    2008
% DN    2008-05-29 Shortened it
% DN    2008-05-31 added cell support

if isstruct(in)
    for p=1:numel(struct)
        qleeg   = structfun(@isempty,in(p));
        fnm     = fieldnames(in(p));
        fnms    = fnm(qleeg).';
        for q=fnms
            in(p).(q{1}) = filling;
        end
    end
elseif iscell(in)
    for p=1:numel(in)
        if isempty(in{p})
            in{p} = filling;
        end
    end
else
    error('input of type %s is not supported',class(in));
end
