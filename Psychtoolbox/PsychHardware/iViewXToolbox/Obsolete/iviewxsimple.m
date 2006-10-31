% iviewx
fprintf([mfilename ' start\n']);
clear all;
try

    pnet('closeall');

    host='192.168.1.2';
    port=4444;
    ivx=initiviewxdefaults([],[], host, port);
    ivx
    [result, ivx]=iViewXComm('open', ivx);
    result
    [result, ivx]=iViewXComm('check', ivx);
    result
    [result, ivx]=iViewX('stoprecording', ivx);

    params='ET_REC';
    [result, ivx]=iViewX('command', ivx, params);
    params='ET_CAL 9';
    [result, ivx]=iViewX('command', ivx, params);

    i=0;
    while i<100
        [data, ivx]=iViewXComm('receive', ivx);

        if ~isempty(data)
           % data
            if strfind(data, 'ET_SPL')
            elseif strfind(data, 'ET_CHG')
                pt=str2num(data(8:end));
                fprintf('Change to point: %d\n', pt);
            elseif strfind(data, 'ET_PNT')
                coords=str2num(data(8:end));
                fprintf('Coords for point %d:\t%d\t%d\n', coords(1), coords(2), coords(3));
            elseif strfind(data, 'ET_CSZ')
                scr_size=str2num(data(8:end));
                fprintf('Screen: %d\t%d\n', scr_size(1), scr_size(2));
            end
        end
       % result
        i=i+1;
        drawnow;
    end
    [result, ivx]=iViewXComm('close', ivx);

    result
    
    [result, ivx]=iViewX('stoprecording', ivx);

    result
    pnet('closeall');
    fprintf([mfilename ' end\n']);

catch
    %this "catch" section executes in case of an error in the "try" section
    %above.  Importantly, it closes the onscreen window if its open.
    pnet('closeall');
    Screen('CloseAll');    rethrow(lasterror);
end %try..catch..

