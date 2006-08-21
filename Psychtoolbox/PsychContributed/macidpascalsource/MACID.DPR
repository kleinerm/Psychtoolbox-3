program macid;
uses
  getmac;

{$IFDEF WIN32}
{$APPTYPE CONSOLE}
{$ENDIF}
{$I-}

{$I ..\LIB\VERSION.INC}

const
  macidversion = 'macid '+BT_VERSION+' for ' + {$IFDEF WIN32} 'win32'{$ELSE}
  'dos'{$ENDIF};
  cleft =
    '(C) markus stephany, own parts released under the terms of the GNU GPL';

  (* /? parameter *)

procedure Usage;
begin
  writeln(macidversion);
  writeln(cleft);
  writeln;
  writeln('macid writes the mac id of the specified (or first) network adapter to stdout.');
  writeln('it uses netbios calls to query the network adapter.');
  writeln;
  writeln('usage:');
  writeln('macid -enum');
  writeln('  enumerates available adapters');
  writeln;
  writeln('macid [<adapter number>] [-8.3] [<variable name>]');
  writeln;
  writeln('  <adapter number>:');
  writeln('    the adapter to query (0-based). if omitted, either the first adapter');
  writeln('    of the adapters enumeration or adapter# 0 is used.');
  writeln;
  writeln('  -8.3:');
  writeln('    if this parameter is specified, the mac address will be printed in 8.3');
  writeln('    format (the leading 0 will be stripped, the last three characters will be');
  writeln('    separated by a dot.)');
  writeln;
  writeln('  <variable name>:');
  writeln('    if a variable name is specified on the command line, the output will be');
  writeln('    modified to a SET command (SET <variable name>=<macid>)');
  writeln;
  writeln('examples:');
  writeln('  macid 0 -8.3 MAC');
  writeln('    will read the mac address of the first adapter and output a set command to');
  writeln('    stdout, mac address in 8.3 format (e.g. "SET MAC=01AC922A.456")');
  writeln;
  writeln('  macid 1');
  writeln('    will read the mac address of the second adapter and write it to stdout');
  writeln('    (e.g. "001AC922A456")');
  writeln;
  Halt(1);
end;

var
  enum: TLanaEnum;

(* retrieve the first adapter in the system (or 0, if enumeration fails) *)
function Get1stAdapter: Byte;
var
  ncb: TNCB;
begin
  (* enumerate the adapters *)
  FillChar(ncb, sizeof(ncb), #0);
  FillChar(enum, sizeof(enum), #0);
  with ncb do
  begin
    Byte(ncb_command) := NCB_ENUM;
    ncb_buffer := addr(enum);
    ncb_length := sizeof(enum);
  end;

  if (Byte(Netbios(ncb)) <> 0) or (enum.length = #0) then
  begin
    enum.length := #0;
    Get1stAdapter := 0;
  end
  else
    Get1stAdapter := Byte(enum.lana[0]);
end;

function ByteToHex(b: byte): string;
const
  HEX: array[0..15] of char = '0123456789ABCDEF';
begin
  bytetohex := HEX[b shr 4] + HEX[b and 15];
end;

var
  status: TNBAdapterStatus;
  ncb: TNCB;
  numadapt,
    pi,
    i: Integer;
  s: string;
begin
  (* check /? parameter *)
  if (ParamStr(1) = '/?') or (ParamStr(1) = '-?') then
    Usage;

  (* enumerate ? *)
  if (ParamStr(1) = '/enum') or (ParamStr(1) = '-enum') then
  begin
    Get1stAdapter;
    if enum.length = #0 then
    begin
      writeln('Error enumerating adapters.');
      Halt(1);
    end
    else
    begin
      for i := 0 to Pred(Ord(enum.length)) do
        write(Ord(enum.lana[i]),' ');
      writeln;
      Halt(0);
    end;
  end;

  (* check the adapter to use *)
  Val(ParamStr(1), numadapt, i);
  if i <> 0 then
  begin
    numadapt := Get1stAdapter;
    pi := 1;
  end
  else
    pi := 2;

  (* reset the adapter *)
  FillChar(ncb, sizeof(ncb), #0);
  with ncb do
  begin
    Byte(ncb_command) := NCB_RESET;
    Byte(ncb_lana_num) := numadapt;
  end;
  if Byte(Netbios(ncb)) <> 0 then
  begin
    writeln('Error resetting adapter ', numadapt, '.');
    Halt(1);
  end;

  (* query adapter status *)
  FillChar(ncb, sizeof(ncb), #0);
  with ncb do
  begin
    Byte(ncb_command) := NCB_ADAPTERSTATUS;
    Byte(ncb_lana_num) := numadapt;
    ncb_buffer := addr(status);
    ncb_length := sizeof(status);
    s := '*               ';
    Move(s[1], ncb_callname[0], NCBNameSize);
  end;
  if Byte(Netbios(ncb)) <> 0 then
  begin
    writeln('Error retrieving the mac address from adapter ', numadapt, '.');
    Halt(1);
  end;

  (* build the mac string *)
  s := '';
  for i := 0 to 5 do
    s := s + ByteToHex(Byte(status.adapter_address[i]));

  (* check other arguments *)
  if ParamStr(pi) <> '' then
  begin
    if ParamStr(pi) = '-8.3' then (* build 8.3 mac adress *)
    begin
      inc(pi);
      s := Copy(s, 2, 8) + '.' + copy(s, 10, 8);
    end;
    if ParamStr(pi) <> '' then
      s := 'set ' + ParamStr(pi) + '=' + s;
    (* make a set var=%mac% output string *)
  end;
  writeln(s);
end.
