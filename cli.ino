
void SetSimpleCli(){

  cmdDebug = cli.addSingleArgCmd("debug");
  cmdDebug.setDescription(" Set level of the debug messages (0 - disable)");

  cmdDevName = cli.addSingleArgCmd("name");
  cmdDevName.setDescription(" Set device name ( doing nothing )");

  cmdSsid = cli.addSingleArgCmd("ssid");
  cmdSsid.setDescription(" Set WiFi SSID");

  cmdPassw = cli.addSingleArgCmd("passw");
  cmdPassw.setDescription(" Set WiFi password");

  cmdHost = cli.addSingleArgCmd("host");
  cmdHost.setDescription(" Set destination host ( hostname or IPv4 )");

  cmdPort = cli.addSingleArgCmd("port");
  cmdPort.setDescription(" Set destination port");

  cmdUri1 = cli.addSingleArgCmd("uri1");
  cmdUri1.setDescription(" Set URI of the informational file");

  cmdUri2 = cli.addSingleArgCmd("uri2");
  cmdUri2.setDescription(" Set URI of the main file");

  cmdHauth = cli.addSingleArgCmd("auth");
  cmdHauth.setDescription(" Set HTTP(S) authorization (0/1, 1=enable)");

  cmdHuser = cli.addSingleArgCmd("huser");
  cmdHuser.setDescription(" Set HTTP(S) username");

  cmdHpassw = cli.addSingleArgCmd("hpassw");
  cmdHpassw.setDescription(" Set HTTP(S) password");

  cmdShow = cli.addSingleArgCmd("show");
  cmdShow.setDescription(" Show configuration");

  cmdSave = cli.addSingleArgCmd("save");
  cmdSave.setDescription(" Save configuration to EEPROM");

  cmdReboot = cli.addSingleArgCmd("reboot");
  cmdReboot.setDescription(" Reboot hard | soft");

  cmdHelp = cli.addSingleArgCmd("help");
  cmdHelp.setDescription(" Get help");

}


void  loop_cli_mode(){
  String input;
  const char emptyArg[] = "Argument is empty, do nothing";
  // uint8_t argNum = 0;
  uint8_t argLen = 0;

  Serial.print("> ");
  readStringWEcho(input, MAX_ALLOWED_INPUT);

  if (input.length() > 0) {
    cli.parse(input);
  }

  if (cli.available()) {
    Command c = cli.getCmd();
    // argNum = c.countArgs();
    argLen = c.getArg(0).getValue().length();
    unsigned int i = 0;
    float j = 0;

    if (c == cmdDebug) {
      if ( argLen == 0 ) {
        Serial.println(emptyArg);
      }else{
        i = c.getArg(0).getValue().toInt();
        if ( i > 255 ) {
          Serial.println("Argument must be from 0 to 255");
        }else{
          debug = (uint8_t)i;
          if ( debug > 0 )
            Serial.printf("Debug printing set to %u level\r\n", debug);
          else
            Serial.println("Debug printing disabled");
        }
      }
    } else if (c == cmdDevName) {
      if ( argLen == 0 ) {
        Serial.println(emptyArg);
      }else{
        memset(dev_name, 0, sizeof(dev_name));
        c.getArg(0).getValue().toCharArray(dev_name, sizeof(dev_name)-1 );
        Serial.printf("Device name set to \"%s\"\r\n",dev_name);
      }
    } else if (c == cmdSsid) {
      if ( argLen == 0 ) {
        Serial.println(emptyArg);
      }else{
        memset(ssid, 0, sizeof(ssid));
        c.getArg(0).getValue().toCharArray(ssid, sizeof(ssid)-1 );
        Serial.println("SSID set to \"" + c.getArg(0).getValue() + "\"");
      }
    } else if (c == cmdPassw) {
      if ( argLen == 0 ) {
        Serial.println(emptyArg);
      }else{
        memset(passw, 0, sizeof(passw));
        c.getArg(0).getValue().toCharArray(passw, sizeof(passw)-1 );
        Serial.println("WiFi password set to \"" + c.getArg(0).getValue() + "\"");
      }
    } else if (c == cmdHost) {
      if ( argLen == 0 ) {
        Serial.println(emptyArg);
      }else{
        memset(host, 0, sizeof(host));
        c.getArg(0).getValue().toCharArray(host, sizeof(host)-1 );
        Serial.println("Host is \"" + c.getArg(0).getValue() + "\"");
      }
    } else if (c == cmdPort) {
      if ( argLen == 0 ) {
        Serial.println(emptyArg);
      }else{
        port = c.getArg(0).getValue().toInt();
        Serial.println("Port set to \"" + c.getArg(0).getValue() + "\"");
      }
    } else if (c == cmdUri1) {
      if ( argLen == 0 ) {
        Serial.println(emptyArg);
      }else{
        memset(uri1, 0, sizeof(uri1));
        c.getArg(0).getValue().toCharArray(uri1, sizeof(uri1)-1 );
        Serial.println("URI of the informational file set to \"" + c.getArg(0).getValue() + "\"");
      }
    } else if (c == cmdUri2) {
      if ( argLen == 0 ) {
        Serial.println(emptyArg);
      }else{
        memset(uri2, 0, sizeof(uri2));
        c.getArg(0).getValue().toCharArray(uri2, sizeof(uri2)-1 );
        Serial.println("URI of the main file set to \"" + c.getArg(0).getValue() + "\"");
      }
    } else if (c == cmdHauth) {
      if ( argLen == 0 ) {
        Serial.println(emptyArg);
      }else{
        i = c.getArg(0).getValue().toInt();
        if ( i > 1 ) {
          Serial.println("Argument must be 0 or 1");
        }else{
          http_auth = (uint8_t)i;
          if ( http_auth == 1 )
            Serial.println("HTTP(S) authorization enabled");
          else
            Serial.println("HTTP(S) authorization disabled");
        }
      }
    } else if (c == cmdHuser) {
      if ( argLen == 0 ) {
        Serial.println(emptyArg);
      }else{
        memset(http_user, 0, sizeof(http_user));
        c.getArg(0).getValue().toCharArray(http_user, sizeof(http_user)-1 );
        Serial.println("HTTP(S) username set to \"" + c.getArg(0).getValue() + "\"");
      }
    } else if (c == cmdHpassw) {
      if ( argLen == 0 ) {
        Serial.println(emptyArg);
      }else{
        memset(http_passw, 0, sizeof(http_passw));
        c.getArg(0).getValue().toCharArray(http_passw, sizeof(http_passw)-1 );
        Serial.println("HTTP(S) password set to \"" + c.getArg(0).getValue() + "\"");
      }
    } else if (c == cmdSave) {
      eeprom_save();
      Serial.println("Configuration saved to EEPROM");
    } else if (c == cmdShow) {
      if ( debug > 0 )
        Serial.printf("Debug printing level set to %u\r\n", debug);
      else
        Serial.println("Debug printing disabled");
      Serial.printf("Device name = \"%s\"\r\n",dev_name);
      Serial.print("WiFi SSID = \"");Serial.print(ssid);Serial.println("\"");
      Serial.print("WiFi password = \"");Serial.print(passw);Serial.println("\"");
      Serial.print("Host = \"");Serial.print(host);Serial.println("\"");
      Serial.print("Port = \"");Serial.print(port);Serial.println("\"");
      Serial.print("URI of the info. file = \"");Serial.print(uri1);Serial.println("\"");
      Serial.print("URI of the main file = \"");Serial.print(uri2);Serial.println("\"");
      if ( http_auth > 0 )
        Serial.println("HTTP(S) authorization enabled");
      else
        Serial.println("HTTP(S) authorization disabled");
      Serial.print("HTTP(S) username = \"");Serial.print(http_user);Serial.println("\"");
      Serial.print("HTTP(S) password = \"");Serial.print(http_passw);Serial.println("\"");
    } else if (c == cmdReboot) {
      Serial.println("Reboot...");
      delay(3000);
      ESP.restart();
    } else if (c == cmdHelp) {
      Serial.println("Help:");
      Serial.println(cli.toString());
    }

  }

  if (cli.errored()) {
    CommandError cmdError = cli.getError();

    Serial.print("ERROR: ");
    Serial.println(cmdError.toString());

    if (cmdError.hasCommand()) {
      Serial.print("Did you mean \"");
      Serial.print(cmdError.getCommand().toString());
      Serial.println("\"?");
    }
  }
}


void readStringWEcho(String& input, size_t char_limit) { // call with char_limit == 0 for no limit
  for(;;) {
    if (Serial.available()) {
      char c = Serial.read();
      if ((uint8_t)c == 8) {
        if ( input.length() ) {
          clearString(input.length());
          input.remove(input.length()-1);
          Serial.print(input);
        }
        continue;
      }
      if ( ((uint8_t)c == 10) || ((uint8_t)c == 13) ){
        Serial.println();
        return;
      }
      if ( ((uint8_t)c < 32) || ((uint8_t)c > 126)) {
        Serial.print((char)7);
        continue;
      }
      input += c;
      Serial.print(c);
      if (char_limit && (input.length() >= char_limit)) {
        return;
      }
    }
  }
}

void clearString( uint16_t len ){
  char stmp[MAX_ALLOWED_INPUT+7];
  memset(stmp+1,' ',len+2);
  stmp[0]='\r';
  stmp[len+3]='\r';
  stmp[len+4]='>';
  stmp[len+5]=' ';
  stmp[len+6]=0;
  Serial.print(stmp);
}
