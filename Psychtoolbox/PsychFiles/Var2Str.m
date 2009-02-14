function str = Var2Str(in,name)
% str = Var2Str(in,name)
%
% Takes variable IN and creates a string representation of it that would
% return the original variable when fed to eval(). NAME is the name of the variable
% that will be printed in this string.
% Can process any (combination of) MATLAB built-in datatype
%
% example:
%   var.field1.field2 = {logical(1),'yar',int32(43),[3 6 1 2],@isempty}
%   Var2Str(var,'var2')
%   ans =
%       var2.field1.field2 = {true,'yar',int32(43),[3,6,1,2],@isempty};

% DN 2008-01    Wrote it.
% DN 2008-07-30 Added support for function handles
% DN 2008-07-31 Added checking of MATLAB version where needed
% DN 2008-08-06 Lessened number of output lines needed to represent
%               variable in some cases
% DN 2008-08-12 Added wronginputhandler() for easy creation of specific
%               error messages on wrong input + added sparse support

% TODO: make extensible by userdefined object parser for user specified
% datatypes - this will make this function complete

% make string of values in input, output is a cell per entry
if isnumeric(in)
    strc = numeric2str(in,name);
elseif islogical(in)
    strc = bool2str(in,name);
elseif ischar(in)
    strc = str2str(in,name);
elseif iscell(in)
    strc = cell2str(in,name);
elseif isstruct(in)
    strc = struct2str(in,name);
elseif isa(in,'function_handle')
    strc = funchand2str(in,name);
else
    wronginputhandler(in);
end

% sort
strc = sort(strc);

% align equals-signs (autistic)
indices     = strfind(strc,'=');
indices     = cellfun(@(x)x(1),indices);
MaxIndex    = max(indices);

for p = 1:length(strc)
    i       = indices(p);
    dif     = MaxIndex - i;
    strc{p} = [strc{p}(1:i-1) repmat(' ',1,dif) strc{p}(i:end)];
end

% make string out of the stringcells
str = [strc{:}];



% functions to deal with the different datatypes
function wronginputhandler(input,name)
inpclass = class(input);
if nargin == 1
    at = '';
else
    at = [char(10) 'Error at ' name];
end
switch inpclass
    case 'inline'
        error('Inline functions not supported, they are deprecated.\nPlease see ''help function_handle'' for information on an alternative.%s',at)
    otherwise
        error('input of type %s is not supported.%s',class(in),at)
end


function str = numeric2str(input,name)

psychassert(isnumeric(input),'numeric2str: Input must be numerical, not %s',class(input))

if isempty(input) || ndims(input)<=2
    if isempty(input)
        if ndims(input)==2 && all(size(input)==0) && isa(input,'double')
            str = '[]';
        else
            s   = size(input);
            str = ['zeros(' regexprep(mat2str(s),'\s+',',')];
            if ~strcmp(class(input),'double')
                str = [str ',''' class(input) ''''];
            end
            str = [str ')'];
        end
    elseif ndims(input)<=2
        if strcmp(class(input),'double')
            str = mat2str(input);
        else
            % for any non-double datatype, preserve type - double is default
            str = mat2str(input,'class');
        end
        str = regexprep(str,'\s+',',');
    end
    if issparse(input)
        str = ['sparse(' str ')'];
    end
    if nargin==2
        str = {[name ' = ' str ';' char(10)]};
    end
else
    psychassert(nargin==2,'input argument name must be defined if processing 2D+ matrix');
    str = mat2strhd(input,name,@numeric2str,[],[]);
end


function str = bool2str(input,name)

psychassert(islogical(input),'bool2str: Input must be logical, not %s',class(input));

if isempty(input) || ndims(input)<=2
    if isempty(input)
        s   = size(input);
        str = ['logical(zeros(' regexprep(mat2str(s),'\s+',',') '))'];
    elseif ndims(input)<=2
        str = mat2str(input);
        str = regexprep(str,'\s+',',');
    end
    if issparse(input)
        str = ['sparse(' str ')'];
    end
    if nargin==2
        str = {[name ' = ' str ';' char(10)]};
    end
else
    psychassert(nargin==2,'input argument name must be defined if processing 2D+ boolean');
    str = mat2strhd(input,name,@bool2str,[],[]);
end

function str = str2str(input,name)

psychassert(ischar(input),'str2str: Input must be char, not %s',class(input));

if isempty(input)
    s   = size(input);
    str = ['char(zeros(' regexprep(mat2str(s),'\s+',',') '))'];

    if nargin==2
        str = {[name ' = ' str ';' char(10)]};
    end
elseif ndims(input)<=2
    nrow    = size(input,1);

    % taken from mat2str, dangerousPattern slightly changed
    strings = cell(nrow,1);
    for row=1:nrow
        strings{row}    = input(row,:);
    end
    dangerousPattern    =  '[\0\r\n\f\v]';
    hasDangerousChars   = regexp(strings, dangerousPattern, 'once');
    
    % for older version of MATLAB, check if the string contains dangerous
    % character, because a dynamic Replacement expression will be needed
    % and this is not supported by R14SP3 or lower
    mver = ver('matlab');
    if ~(str2num(mver.Version)>7.1)
        psychassert(~any(cellfun(@(x)~isempty(x),hasDangerousChars)),'Need at least MATLAB R2006a when string contains dangerous characters (\0\r\n\f\v)');
    end

    needsConcatenation  = nrow > 1 | ~isempty([hasDangerousChars{:}]);

    strings     = strrep(strings, '''', '''''');
    strings     = regexprep(strings, dangerousPattern, ''' char(${sprintf(''%d'',$0)}) ''');

    if needsConcatenation
        str = '[';
    else
        str = '';
    end

    str = [str '''' strings{1} ''''];

    for row = 2:nrow
        str = [str ';''' strings{row} ''''];
    end

    if needsConcatenation
        str = [str ']'];
    end

    if nargin==2
        str = {[name ' = ' str ';' char(10)]};
    end
else
    psychassert(nargin==2,'input argument name must be defined if processing 2D+ str');
    str = mat2strhd(input,name,@str2str,[],[]);
end

function str = cell2str(input,name)

psychassert(iscell(input),'cell2str: Input must be cell, not %s',class(input));

qstruct     = IsACell(input,@isstruct);         % recursively check if there is any struct in the cell
q2dplus     = IsACell(input,@(x)ndims(x)>2);    % recursively check if there is any element in the cell extending over more than 2 dimensions


if isempty(input)
    if ndims(input)==2 && all(size(input)==0)
        str = '{}';
    else
        s   = size(input);
        str = ['cell(zeros(' regexprep(mat2str(s),'\s+',',') '))'];
    end

    if nargin==2
        str = {[name ' = ' str ';' char(10)]};
    end
elseif ~qstruct && ~q2dplus
    [nrow ncol] = size(input);
    str         = '{';
    % process cell per element
    for p = 1:nrow
        for q=1:ncol
            if isnumeric(input{p,q})
                str = [str numeric2str(input{p,q})];
            elseif islogical(input{p,q})
                str = [str bool2str(input{p,q})];
            elseif ischar(input{p,q})
                str = [str str2str(input{p,q})];
            elseif iscell(input{p,q})
                str = [str cell2str(input{p,q})];
            elseif isstruct(input{p,q})
                % if there is a struct, cell2strhd() should have been handling this
                error('structs should not be processed here in any circumstance');
            elseif isa(input{p,q},'function_handle')
                str = [str funchand2str(input{p,q})];
            else
                wronginputhandler(input{p,q});
            end
            if q~=ncol
                str = [str ','];
            end
        end
        if p~=nrow
            str = [str ';'];
        end
    end
    str = [str '}'];

    if nargin==2
        str = {[name ' = ' str ';' char(10)]};
    end
else
    psychassert(nargin==2,'input argument name must be defined if processing 2D+ cell');
    str = cell2strhd(input,name,[],[]);
end

function strc = struct2str(input,name)

if ~isstruct(input)
    error('Input is not struct')
end

%%%%%
qnotscalar  = any(size(input)>1);
strc        = [];

if ~qnotscalar
    velden = fieldnames(input);
    for r=1:length(velden)
        wvar = input.(velden{r});
        namesuff = ['.' velden{r}];

        if isnumeric(wvar)
            strc = [strc; numeric2str(wvar,[name namesuff])];
        elseif islogical(wvar)
            strc = [strc; bool2str(wvar,[name namesuff])];
        elseif ischar(wvar)
            strc = [strc; str2str(wvar,[name namesuff])];
        elseif iscell(wvar)
            strc = [strc; cell2str(wvar,[name namesuff])];
        elseif isstruct(wvar)
            strc = [strc; struct2str(wvar,[name namesuff])];
        elseif isa(wvar,'function_handle')
            strc = [strc; funchand2str(wvar,[name namesuff])];
        else
            wronginputhandler(wvar,[name namesuff]);
        end
    end
else
    psychassert(nargin==2,'input argument name must be defined if processing 2D+ struct');
    strc = struct2strhd(input,name,[],[]);
end



function str = funchand2str(input,name)

psychassert(isa(input,'function_handle'),'funchand2str: Input must be a function handle, not %s',class(input));

str = func2str(input);
if str(1)~='@'
    str = ['@' str];
end
if nargin==2
    str = {[name ' = ' str ';' char(10)]};
end



%%%% HD functies for variables of more than 2 non-singleton dimensions

function strc = mat2strhd(in,name,fhndl,ndim,adrmat)

if isempty(ndim)
    ndim = ndims(in);                       % ndim is equal to number highest dim
end
if isscalar(unique(in))
    % special case
    item = unique(in);
    s    = size(in);
    if ~isempty(adrmat)
        if length(adrmat)>1
            mid = Interleave(repmat(',',1,length(adrmat)),adrmat(end:-1:1));
        else
            mid = [',' num2str(adrmat)];
        end
    else
        mid = '';
    end
    vname = [name '(' Interleave(repmat(':',1,ndim),repmat(',',1,ndim-1)) mid ') = '];
    sizestr = regexprep(mat2str(s),'\s+',',');
    if item == 0
        mid = ['zeros(' sizestr ')'];
        if ~isa(item,'double')
            mid = [class(in) '(' mid ');'];
        end
    else
        mid = ['ones(' sizestr ')'];
        if item ~= 1
            mid = [fhndl(item) '*' mid];
        end
        if ~isa(item,'double')
            mid = [class(in) '(' mid ');'];
        end
    end
    strc = {[vname mid ';' char(10)]};
    return;
end

if ndim==2
    if length(adrmat)>1
        mid = Interleave(adrmat(end:-1:1),repmat(',',1,length(adrmat)-1));
    else
        mid = num2str(adrmat);
    end
    namesuff = ['(:,:,' mid ')'];
    strc = fhndl(in,[name namesuff]);       % dispatch 2D to correct interpreter
else
    strc    = [];
    s       = AltSize(in,[1:ndim-1]);
    ind     = prod(s);

    for p=1:size(in,ndim)
        mat = in((p-1)*ind+1:p*ind);        % calculate linear indices and get data to be processed
        input = reshape(mat,s);             % reshape back into original shape
        strc = [strc; mat2strhd(input,name,fhndl,ndim-1,[adrmat p])];
    end
end

function strc = cell2strhd(in,name,ndim,adrmat)

if isempty(ndim)
    ndim = ndims(in);        % ndim is equal to number highest dim
end

strc = [];
if ndim==1
    for p=1:length(in)
        adrmatt  = [adrmat p];
        namesuff = ['{' Interleave(adrmatt(end:-1:1),repmat(',',1,length(adrmatt)-1)) '}'];
        if ndims(in{p})==2 && all(size(in{p})==0) && isa(in{p},'double')
            continue;
        elseif isnumeric(in{p})
            strc = [strc; numeric2str(in{p},[name namesuff])];
        elseif islogical(in{p})
            strc = [strc; bool2str(in{p},[name namesuff])];
        elseif ischar(in{p})
            strc = [strc; str2str(in{p},[name namesuff])];
        elseif iscell(in{p})
            strc = [strc; cell2str(in{p},[name namesuff])];
        elseif isstruct(in{p})
            strc = [strc; struct2str(in{p},[name namesuff])];
        elseif isa(in{p},'function_handle')
            strc = [strc; funchand2str(in{p},[name namesuff])];
        else
            wronginputhandler(in{p},[name namesuff]);
        end
    end
else
    s   = AltSize(in,[1:ndim-1]);
    ind = prod(s);

    for p=1:size(in,ndim)
        mat = in((p-1)*ind+1:p*ind); % calculate linear indices and get data to be processed
        if isscalar(s)
            input = mat;
        else
            input = reshape(mat,s);  % reshape back into original shape
        end
        strc = [strc; cell2strhd(input,name,ndim-1,[adrmat p])];
    end
end

function strc = struct2strhd(in,name,ndim,adrmat)

if isempty(ndim)
    ndim = ndims(in);        % ndim is equal to number highest dim
end

strc = [];
if ndim==1
    for p=1:length(in)
        if all(structfun(@(x)ndims(x)==2 && all(size(x)==0) && isa(x,'double'),in(p)))
            continue;
        else
            adrmatt = [adrmat p];
            namesuff = ['(' Interleave(adrmatt(end:-1:1),repmat(',',1,length(adrmatt)-1)) ')'];
            strc = [strc; struct2str(in(p),[name namesuff])];
        end
    end
else
    s = AltSize(in,[1:ndim-1]);
    ind = prod(s);

    for p=1:size(in,ndim)
        mat = in((p-1)*ind+1:p*ind); % calculate linear indices and get data to be processed
        if isscalar(s)
            input = mat;
        else
            input = reshape(mat,s);  % reshape back into original shape
        end
        strc = [strc; struct2strhd(input,name,ndim-1,[adrmat p])];
    end
end