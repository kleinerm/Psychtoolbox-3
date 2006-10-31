    host='192.168.1.2';
    port=4444;
    ivx=initiviewxdefaults([],[], host, port);
    ivx

    params=['ET_CSZ ' num2str(1280) ' ' num2str(1024)]
    [result, ivx]=iViewX('command', ivx, params);
    
