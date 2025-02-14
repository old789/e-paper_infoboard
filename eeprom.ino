
void eeprom_save(){
  const char msg[] = "Config saved to EEPROM";
  EEPROM.put(0, mark);
  EEPROM.put(PT_DEV_NAME, dev_name);
  EEPROM.put(PT_SSID, ssid);
  EEPROM.put(PT_PASSW, passw);
  EEPROM.put(PT_HOST, host);
  EEPROM.put(PT_PORT, port);
  EEPROM.put(PT_URI1, uri1);
  EEPROM.put(PT_URI2, uri2);
  EEPROM.put(PT_AUTH, http_auth);
  EEPROM.put(PT_HUSER, http_user);
  EEPROM.put(PT_HPASSW, http_passw);
  EEPROM.put(PT_DEBUG, debug);
  EEPROM.put(PT_CRC, ram_crc());
  EEPROM.commit();
  if ( debug > 0 )
    Serial.println(msg);
}

unsigned long ram_crc() {
  unsigned long crc = ~0L;
  const char msg[] = "Can't allocate memory";
  const unsigned long crc_table[16] = {
    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
    0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
  };
  byte *buf = (byte*)malloc( SIZE_EEPROM + 8 );

  if ( ! buf ){
    if ( enable_cli or debug > 0 )
      Serial.println(msg);
  return(0);
  }

  memset(buf, 0, SIZE_EEPROM + 8);
  memcpy(buf, &mark, sizeof(mark));
  memcpy(buf+PT_DEV_NAME, &dev_name, strlen(dev_name));
  memcpy(buf+PT_SSID, &ssid, strlen(ssid));
  memcpy(buf+PT_PASSW, &passw, strlen(passw));
  memcpy(buf+PT_HOST, &host, strlen(host));
  memcpy(buf+PT_PORT, &port, sizeof(port));
  memcpy(buf+PT_URI1, &uri1, strlen(uri1));
  memcpy(buf+PT_URI2, &uri2, strlen(uri2));
  memcpy(buf+PT_AUTH, &http_auth, sizeof(http_auth));
  memcpy(buf+PT_HUSER, &http_user, strlen(http_user));
  memcpy(buf+PT_HPASSW, &http_passw, strlen(http_passw));
  memcpy(buf+PT_DEBUG, &debug, sizeof(debug));

  for (uint16_t index = 0 ; index <= SIZE_EEPROM  ; ++index) {
    crc = crc_table[(crc ^ buf[index]) & 0x0f] ^ (crc >> 4);
    crc = crc_table[(crc ^ (buf[index] >> 4)) & 0x0f] ^ (crc >> 4);
    crc = ~crc;
  }
  free(buf);
  return crc;
}

bool eeprom_read(){
uint16_t m = 0;
unsigned long crc = 0;
const char msg11[] = "EEPROM read ERROR: incorrect marker, need ";
const char msg12[] = "H but read ";
const char msg13[] = "H";
const char msg2[] = "EEPROM read ERROR: incorrect crc";
const char msg3[] = "EEPROM read successfully";

  EEPROM.get(0, m);
  if ( m != mark ) {
    if ( enable_cli or debug > 0 ) {
      Serial.print(msg11);Serial.print(mark,HEX);Serial.print(msg12);Serial.print(m,HEX);Serial.println(msg13);
    }
    return(false);
  }

  EEPROM.get(PT_DEV_NAME, dev_name);
  EEPROM.get(PT_SSID, ssid);
  EEPROM.get(PT_PASSW, passw);
  EEPROM.get(PT_HOST, host);
  EEPROM.get(PT_PORT, port);
  EEPROM.get(PT_URI1, uri1);
  EEPROM.get(PT_URI2, uri2);
  EEPROM.get(PT_CRC, crc);
  EEPROM.get(PT_AUTH, http_auth);
  EEPROM.get(PT_HUSER, http_user);
  EEPROM.get(PT_HPASSW, http_passw);
  EEPROM.get(PT_DEBUG, debug);

  if ( crc != ram_crc() ){
    if ( enable_cli or debug > 0 )
      Serial.println(msg2);
    return(false);
  }
  if ( debug > 0 )
    Serial.println(msg3);
  return(true);
}

bool is_conf_correct(){
  const char msg1[] = "Config incorrect";
  const char msg2[] = "Config good";

  if (( strlen(ssid) == 0 ) ||
      ( strlen(passw) == 0 ) ||
      ( strlen(host) == 0 ) ||
      ( port == 0 ) ||
      ( strlen(uri1) == 0 ) ||
      ( strlen(uri2) == 0 )
      ){
    if ( debug > 0 )
      Serial.println(msg1);
    return(false);
  } else if ( http_auth == 1 ) {
    if (( strlen(http_user) == 0 ) ||
        ( strlen(http_passw) == 0 )
        ){
      if ( debug > 0 )
        Serial.println(msg1);
      return(false);
    }
  }

  if ( debug > 0 )
    Serial.println(msg2);
  return(true);
}
