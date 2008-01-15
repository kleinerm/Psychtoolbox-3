function DaqCalls
% Examples for each function in the Daq Toolbox.
% * Analog input/output commands  
% [data,params]=DaqAIn(DeviceIndex,options);                   % Read analog in (works)
% [data,params]=DaqAInScan(DeviceIndex,options);               % Scan analog channels (works)
%           err=DaqAInStop(DeviceIndex);                       % Stop input scan (works)
%        params=DaqAInScanBegin(DeviceIndex,options);          % Begin sampling. (works)
%        params=DaqAInScanContinue(DeviceIndex,options);       % Continue sampling: transfer data from Mac OS to PsychHID. (works)
% [data,params]=DaqAInScanEnd(DeviceIndex,options);            % End sampling: data are returned. (works)
%           err=DaqALoadQueue(DeviceIndex,channel,gain);       % Set channel gains (works)
%           err=DaqAOut(DeviceIndex,channel,v);                % Write analog out (does not work with 1608FS -- no analog output)
%        params=DaqAOutScan(DeviceIndex,v,options);            % Clocked analog out (does not work with 1608FS -- no analog output)
%           err=DaqAOutStop(DeviceIndex);                      % Stop output scan (does not work with 1608FS -- no analog output)
% 
% * Digital input/output commands 
%           err=DaqDConfigPort(DeviceIndex,port,direction);    % Configure digital port (works)
%          data=DaqDIn(DeviceIndex);                           % Read digital ports (works)
%           err=DaqDOut(DeviceIndex,port,data);                % Write digital port (works)
%           err=DaqDConfigPortBit(DeviceIndex,BitNo,direction);% Configure individual port bits (works on 1608FS only; also beware: physical input overrides config)
%      BitValue=DaqDReadBit(DeviceIndex,BitNo);                % Read single bit from digital port (works on 1608FS only)
%           err=DaqDWriteBit(DeviceIndex,BitNo);               % Write single bit to digital port (works on 1608FS only)
%
% * Miscellaneous commands    
%           daq=DaqDeviceIndex;                                % Get reference(s) to our device(s); (works)
%           daq=DaqFind;                                       % Return DeviceIndex iff one device is connected (works)
%           err=DaqBlinkLED(DeviceIndex);                      % Cause LED to blink (works)
%           err=DaqCInit(DeviceIndex);                         % Initialize counter (works)
%         count=DaqCIn(DeviceIndex);                           % Read counter (works)
%          data=DaqGetAll(DeviceIndex);                        % Retrieve all analog and digital input values (does not work on 1608FS)
%        status=DaqGetStatus(DeviceIndex);                     % Retrieve device status (works)
%           err=DaqReset(DeviceIndex);                         % Reset the device (sort of works)
%           err=DaqSetCal(DeviceIndex,on);                     % Set CAL output (works)
%               DaqCalibrateAIn(DeviceIndex,channel);          % Compare input/output and write calibration data to file (1608FS only)
%           err=DaqSetSync(DeviceIndex,type);                  % Configure sync (works)
%           err=DaqSetTrigger(DeviceIndex,rising);             % Configure ext. trigger (works)
%  
% * Memory commands   
%          data=DaqMemRead(DeviceIndex,address,bytes);         % Read memory (works)
%           err=DaqMemWrite(DeviceIndex,address,data);         % Write memory (works)
%          data=DaqReadCode(DeviceIndex,address,bytes);        % Read program memory (works)
%           err=DaqPrepareDownload(DeviceIndex);               % Prepare for program memory download (works)
%           err=DaqWriteCode(DeviceIndex,address,data);        % Write program memory (not adequately tested)
% For 1608 DaqWriteCode probably cannot work except in conjunction with a DaqUpdateCode function no one has written
%           err=DaqWriteSerialNumber(DeviceIndex,serialstring);% Write a new serial number to device (not adequately tested)
%
% See also Daq, DaqFunctions, DaqCodes, DaqPins, and DaqHelp.
return;                                                                                                                        
