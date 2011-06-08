function success = FileVar2Str

success = true;

try
    % create some simple and complicated variables, Var2Str should be able
    % to handle all of them.
    a = [3 4 5];
    b = [true false];
    c = char([116 101 115 116 32 13]);
    d = @sin;
    e = @(x) sin(x);    % anonymous function handle
    f = {1,2, [3 4 5], true, uint32([335321 32]), zeros([0, 3],'int16')};
    f{50,1,30,45} = {6,7,8};
    g.f1 = true;
    g(5).get = 'get';
    g(30).cell{1,4,62,2,5}.test(45).cell2{3,3,54,2,1} = {4,7,'test',@exp};
    % compression test (above we already that default-empty values are not
    % output, now test if fields of the same number (constant arrays) are
    % compressed
    h{1}  = {[6 6 6 6 6]};
    h{5}  = {int32([6 6 6 6 6])};
    h{9}  = false(5,7,3,2);
    h{13} = repmat('a',[3,4,23,2,7]);
    h{15} = zeros(30,30,5);
    h{18} = zeros(30,30,5,'uint16');
    h{19} = sprintf('\n\r');
    h{21} = speye(7);
    
    % when evaling the output of Var2Str, we should get exactly the same
    % data back, lets do it
    success = success && Var2StrTester(a,'a');
    success = success && Var2StrTester(b,'b');
    success = success && Var2StrTester(c,'c');
    success = success && Var2StrTester(d,'d');
    success = success && Var2StrTester(f,'f');
    success = success && Var2StrTester(g,'g');
    success = success && Var2StrTester(h,'h');
    
    % anonymous function handles won't compare equal, so need to deal with
    % this by hand
    str = Var2Str(e,'e2');
    eval(str);
    f1 = functions(e);
    f2 = functions(e2);
    if ~strcmp(f1.function,f2.function(end+[-length(f1.function)+1:0]))
        success = false;
        fprintf('Var2Str failed on variable e\n');
    end
    
    % check the simple one input version works as well
    if ~isequal(a,eval(Var2Str(a)))
        success = false;
        fprintf('Var2Str failed with 1 input version\n');
    end
    
catch
    success = false;
    fprintf('error ocurred: "%s"\n',lasterr);
    
end









function [success,var2,str] = Var2StrTester(var,name)
str = Var2Str(var,'var2');
eval(str);
if ~isequal(var,var2)
    success = false;
    fprintf('Var2Str failed on variable %s\n',name);
else
    success = true;
end