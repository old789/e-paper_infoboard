unsigned long read_info_file(){
unsigned long payload;

  if(WiFi.status() != WL_CONNECTED){
    if ( debug > 0 )
      Serial.println("WiFi Disconnected");
    wifi_init();
  }

  if ( debug > 1 )
    Serial.println("Receiving infofile");

  NetworkClientSecure *client = new NetworkClientSecure;
  if (!client) {
    if ( debug > 0 )
      Serial.println("Unable to create client");
    return(0);
  }
  client->setInsecure();
  HTTPClient https;

  if ( debug > 4 )
    Serial.println("[HTTPS] begin...");
  if (!https.begin(*client, host, port, uri1, true)) {  // HTTPS
    if ( debug > 0 )
      Serial.println("[HTTPS] Unable to connect");
    return(0);
  }
  if ( http_auth > 0 ) {
    https.setAuthorization(http_user, http_passw);
  }
  if ( debug > 4 )
    Serial.println("[HTTPS] GET...");
  int httpCode = https.GET();
  if ( debug > 4 )
    Serial.printf("[HTTPS] GET... code: %d\r\n", httpCode);

  if (httpCode < 0) {
    return(0);
  }
  if ( httpCode != HTTP_CODE_OK ) {
  if ( debug > 0 )
    Serial.printf("[HTTPS] GET... failed, error: %s\r\n", https.errorToString(httpCode).c_str());
    return(0);
  }
  payload = https.getString().toInt();
  if ( debug > 1 )
    Serial.printf("[HTTPS] GET... success: %u\r\n", payload);

  https.end();
  delete client;
  return(payload);
}

bool read_data(){

  //Check WiFi connection status
  if(WiFi.status() != WL_CONNECTED){
    if ( debug > 0 )
      Serial.println("WiFi Disconnected");
    wifi_init();
  }

  if ( debug > 1 )
    Serial.println("Receive data");

  NetworkClientSecure *client = new NetworkClientSecure;
  if (!client) {
    if ( debug > 0 )
      Serial.println("Unable to create client");
    return(false);
  }
  client->setInsecure();

  // Add a scoping block for HTTPClient https to make sure it is destroyed before NetworkClientSecure *client is
  HTTPClient https;

  if ( debug > 4 )
    Serial.println("[HTTPS] begin...");
  if (!https.begin(*client, host, port, uri2, true)) {  // HTTPS
    if ( debug > 0 )
      Serial.println("[HTTPS] Unable to connect");
    return(false);
  }
  if ( debug > 4 )
    Serial.println("[HTTPS] GET...");
  // start connection and send HTTP header
  int httpCode = https.GET();

  // HTTP header has been send and Server response header has been handled
  if ( debug > 4 )
    Serial.printf("[HTTPS] GET... code: %d\r\n", httpCode);
  // httpCode will be negative on error
  if (httpCode < 0) {
    return(false);
  }
  // file found at server
  if ( httpCode != HTTP_CODE_OK ) {
    if ( debug > 0 )
      Serial.printf("[HTTPS] GET... failed, error: %s\r\n", https.errorToString(httpCode).c_str());
    return(false);
  }

  // get length of document (is -1 when Server sends no Content-Length header)
  int len = https.getSize();
  if ( debug > 4 )
    Serial.printf("[HTTPS] GET... size: %d\r\n", len);

  // create buffer for read
  uint8_t buff[1024] = {0};
  uint32_t curr_len=0;
  uint32_t chunks=0;

  // get tcp stream
  NetworkClient *stream = https.getStreamPtr();
  if ( debug > 4 )
    Serial.println("[HTTPS] Download begun...");

  // read all data from server
  while (https.connected() && (len > 0 || len == -1)) {
    // get available data size
    size_t size = stream->available();

    if (size) {
      int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));

      chunks++;
      if ( debug > 8 )
        Serial.printf("[HTTPS] copy %d done %d from %d\r\n", c, curr_len, Imagesize);

      if ( (curr_len + c) <= Imagesize ) {
        memcpy(UsualImage+curr_len, buff, c);
        curr_len+=c;
      }

      if (len > 0) {
        len -= c;
      }

    }
  }
  if ( debug > 4 )
    Serial.printf("[HTTPS] downlod done: %u from %d by %u chunks\r\n", curr_len, Imagesize, chunks);
  https.end();

  delete client;

  return(true);
}

void wifi_init(){
  uint16_t wifi_tries = 0;
  if ( debug > 1 )
    Serial.printf("Connecting to %s ...\r\n", ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, passw);             // Connect to the network

  uint16_t i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(100);
    i++;
    if ( debug > 1 )
      Serial.printf("%d ",i);
    if ( i > 1500 ) {  // if don't connect - wait, then next try
      if ( ++wifi_tries > 3 ) {
        ESP.restart();
      }
      delay(150000);
      if ( debug > 1 )
        Serial.printf("Try %d connect to WiFi\r\n",wifi_tries);
    }
  }

  if ( wifi_tries != 0 ) {
    wifi_tries=0;
  }

  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

  if ( debug > 0 )
    Serial.printf("\r\nConnection established! IP address: %s, RSSI: %d\r\n", WiFi.localIP().toString().c_str(), WiFi.RSSI());
}
