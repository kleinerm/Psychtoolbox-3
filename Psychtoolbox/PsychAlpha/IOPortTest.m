% IOPortTest
%
% Try out the new IOPort function which uses the IO Warrior USB device from
% CodeMercenaries



allOn=ones(1,32);
allOff=zeros(1,32);

t1=GetSecs;
IOPort('Init')
GetSecs-t1
for i=1:10
    t1=GetSecs;
    IOPort('SetPort', allOn);
    GetSecs-t1
    WaitSecs(0.5);
    IOPort('SetPort', allOff);
    WaitSecs(0.5);
end


   
