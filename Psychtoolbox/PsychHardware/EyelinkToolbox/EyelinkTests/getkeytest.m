clear all;
commandwindow;
fprintf('OSX Eyelink GetKeys Test\n\n\t');
% input('Hit the return key to continue.','s');
% fprintf('Thanks.\n');


el=EyelinkInitDefaults;

start=GetSecs;
while 1
    [key, el]=EyelinkGetKey(el);
    if key==el.TERMINATE_KEY
        break;
    end
    if GetSecs-start>1000
        fprintf('EyelinkGetKey timed-out.\n');
        break;
    end
    if key~=el.JUNK_KEY & key ~= 0
        key
        fprintf('Eyelink key %s (%.1f)\n', char(key), GetSecs-start);
    end
end

FlushEvents;

fprintf('End of Eyelink GetKeys Test.\n');

