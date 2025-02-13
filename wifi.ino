unsigned long read_info_file(){
unsigned long payload;

  if(WiFi.status() != WL_CONNECTED){
#ifdef DBG_WIFI
    Serial.println("WiFi Disconnected");
#endif
    wifi_init();
  }

#ifdef DBG_WIFI
  Serial.println("Receiving infofile");
#endif

  NetworkClientSecure *client = new NetworkClientSecure;
  if (!client) {
#ifdef DBG_WIFI
    Serial.println("Unable to create client");
#endif
    return(0);
  }
  client->setInsecure();
  HTTPClient https;

#ifdef DBG_WIFI
  Serial.println("[HTTPS] begin...");
#endif
  if (!https.begin(*client, host, port, uri1, true)) {  // HTTPS
#ifdef DBG_WIFI
    Serial.println("[HTTPS] Unable to connect");
#endif
    return(0);
  }
#ifdef DBG_WIFI
  Serial.println("[HTTPS] GET...");
#endif
  int httpCode = https.GET();
#ifdef DBG_WIFI
  Serial.printf("[HTTPS] GET... code: %d\r\n", httpCode);
#endif

  if (httpCode < 0) {
    return(0);
  }
  if ( httpCode != HTTP_CODE_OK ) {
#ifdef DBG_WIFI
    Serial.printf("[HTTPS] GET... failed, error: %s\r\n", https.errorToString(httpCode).c_str());
#endif
    return(0);
  }
  payload = https.getString().toInt();
#ifdef DBG_WIFI
    Serial.printf("[HTTPS] GET... success: %u\r\n", payload);
#endif

  https.end();
  delete client;
  return(payload);
}

bool read_data(){

  //Check WiFi connection status
  if(WiFi.status() != WL_CONNECTED){
#ifdef DBG_WIFI
    Serial.println("WiFi Disconnected");
#endif
    wifi_init();
  }

#ifdef DBG_WIFI
  Serial.println("Receive data");
#endif

  NetworkClientSecure *client = new NetworkClientSecure;
  if (!client) {
#ifdef DBG_WIFI
    Serial.println("Unable to create client");
#endif
    return(false);
  }
  //client->setCACert(rootCACertificate);
  client->setInsecure();

  // Add a scoping block for HTTPClient https to make sure it is destroyed before NetworkClientSecure *client is
  HTTPClient https;

#ifdef DBG_WIFI
  Serial.println("[HTTPS] begin...");
#endif
  if (!https.begin(*client, host, port, uri2, true)) {  // HTTPS
#ifdef DBG_WIFI
    Serial.println("[HTTPS] Unable to connect");
#endif
    return(false);
  }
#ifdef DBG_WIFI
  Serial.println("[HTTPS] GET...");
#endif
  // start connection and send HTTP header
  int httpCode = https.GET();

  // HTTP header has been send and Server response header has been handled
#ifdef DBG_WIFI
  Serial.printf("[HTTPS] GET... code: %d\r\n", httpCode);
#endif
  // httpCode will be negative on error
  if (httpCode < 0) {
    return(false);
  }
  // file found at server
  if ( httpCode != HTTP_CODE_OK ) {
#ifdef DBG_WIFI
    Serial.printf("[HTTPS] GET... failed, error: %s\r\n", https.errorToString(httpCode).c_str());
#endif
    return(false);
  }

  // get length of document (is -1 when Server sends no Content-Length header)
  int len = https.getSize();
#ifdef DBG_WIFI
  Serial.printf("[HTTPS] GET... size: %d\r\n", len);
#endif

  // create buffer for read
  uint8_t buff[1024] = {0};
  uint32_t curr_len=0;
  uint32_t chunks=0;

  // get tcp stream
  NetworkClient *stream = https.getStreamPtr();
#ifdef DBG_WIFI
  Serial.printf("[HTTPS] Download begun...\r\n");
#endif

  // read all data from server
  while (https.connected() && (len > 0 || len == -1)) {
    // get available data size
    size_t size = stream->available();

    if (size) {
      int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));

#ifdef DBG_WIFI
      chunks++;
      //Serial.printf("[HTTPS] copy %d done %d from %d\r\n", c, j, Imagesize);
#endif

      if ( (curr_len + c) <= Imagesize ) {
        memcpy(UsualImage+curr_len, buff, c);
        curr_len+=c;
      }

      if (len > 0) {
        len -= c;
      }

    }
  }
#ifdef DBG_WIFI
  Serial.printf("[HTTPS] downlod done: %u from %d by %u chunks\r\n", curr_len, Imagesize, chunks);
#endif
  https.end();

  delete client;

  return(true);
}

void wifi_init(){
  uint16_t wifi_tries = 0;
#ifdef DBG_WIFI
  Serial.print("Connecting to "); Serial.print(ssid); Serial.println(" ...");
#endif

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, passw);             // Connect to the network

  uint16_t i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(100);
    i++;
#ifdef DBG_WIFI
    Serial.print(i); Serial.print(' ');
#endif
    if ( i > 1500 ) {  // if don't connect - wait, then next try
      if ( ++wifi_tries > 3 ) {
        ESP.restart();
      }
      delay(150000);
#ifdef DBG_WIFI
      Serial.print("Try "); Serial.print(wifi_tries); Serial.println(" connect to WiFi");
#endif
    }
  }

  if ( wifi_tries != 0 ) {
    wifi_tries=0;
  }

  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

#ifdef DBG_WIFI
  Serial.println('\n');
  Serial.println("Connection established!");
  Serial.print("IP address: ");Serial.println(WiFi.localIP());
  Serial.print("RSSI: ");Serial.println(WiFi.RSSI());
#endif
}
