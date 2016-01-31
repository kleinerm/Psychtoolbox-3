clear;
commandwindow;
Eyelink('ShutDown');

if (Eyelink('Initialize') ~= 0)	return;

    fprintf('Problem initializing eyelink\n');
end;
el=EyelinkInitDefaults();
WaitSecs(2);

key='r';
double(key)
Eyelink('SendKeyButton', double(key), 0, el.KB_PRESS );

WaitSecs(2);

key='l';
double(key)
Eyelink('SendKeyButton', double(key), 0, el.KB_PRESS );
WaitSecs(2);

key='r';
double(key)
Eyelink('SendKeyButton', double(key), 0, el.KB_PRESS );
WaitSecs(2);

key='l';
double(key)
Eyelink('SendKeyButton', double(key), 0, el.KB_PRESS );

WaitSecs(2);

Eyelink('ShutDown');
