function DaqCalls
% Examples for each function in the Daq Toolbox.
% * Analog input/output commands  
% [data,params]=DaqAIn(device,options);                   % Read analog in
% [data,params]=DaqAInScan(device,options);               % Scan analog channels
%           err=DaqAInStop(device);                       % Stop input scan
%        params=DaqAInScanBegin(device,options);          % Begin sampling.
%        params=DaqAInScanContinue(device,options);       % Continue sampling: transfer data from Mac OS to PsychHID.
% [data,params]=DaqAInScanEnd(device,options);            % End sampling: data are returned.
%           err=DaqALoadQueue(device,channel,gain);       % Set channel gains
%           err=DaqAOut(device,channel,v);                % Write analog out
%        params=DaqAOutScan(device,v,options);            % Clocked analog out
%           err=DaqAOutStop(device);                      % Stop output scan
% 
% * Digital input/output commands 
%           err=DaqDConfigPort(device,port,direction);    % Configure digital port
%          data=DaqDIn(device);                           % Read digital ports
%           err=DaqDOut(device,port,data);                % Write digital port
%  
% * Miscellaneous commands    
%           daq=DaqDeviceIndex                            % Get reference(s); to our device(s);
%           err=DaqBlinkLED(device);                      % Cause LED to blink
%           err=DaqCInit(device);                         % Initialize counter
%         count=DaqCIn(device);                           % Read counter
%          data=DaqGetAll(device);                        % Retrieve all analog and digital input values
%        status=DaqGetStatus(device);                     % Retrieve device status
%           err=DaqReset(device);                         % Reset the device
%           err=DaqSetCal(device,on);                     % Set CAL output
%           err=DaqSetSync(device,type);                  % Configure sync
%           err=DaqSetTrigger(device,rising);             % Configure ext. trigger
%  
% * Memory commands   
%          data=DaqMemRead(device,address,bytes);         % Read memory
%           err=DaqMemWrite(device,address,data);         % Write memory
%           err=DaqPrepareDownload(device);               % Read program memory
%          data=DaqReadCode(device,address,bytes);        % Prepare for program memory download
%           err=DaqWriteCode(device,address,data);        % Write program memory
%           err=DaqWriteSerialNumber(device,serialstring);% Write a new serial number to device
% See also Daq, DaqFunctions.