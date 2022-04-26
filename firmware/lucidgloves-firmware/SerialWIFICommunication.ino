//only compiles if BTSerial is set because it won't compile for a non-compatible board
#if COMMUNICATION == COMM_WIFISERIAL
#include <WiFi.h>

#define WIFI_READ_BUFFER_LEN 100
class WIFISerialCommunication : public ICommunication {
  private:
    bool m_isOpen;
    WiFiServer m_server{WIFISERIAL_PORT};
    WiFiClient m_client;
    char m_buffer[WIFI_READ_BUFFER_LEN];
    int m_buffer_pos = 0;
    
  public:
    WIFISerialCommunication() {
      m_isOpen = false;
    }

    bool isOpen(){
      return m_isOpen;
    }

    void start(){
      Serial.begin(115200);
      WiFi.mode(WIFI_STA);
      WiFi.begin(WIFISERIAL_SSID, WIFISERIAL_PASSWORD);
      if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.printf("WiFI connection failed!\n");
        return;
      }
       else {
        Serial.println("Your board is now connected to: ");
        Serial.println(WiFi.localIP());
        m_isOpen = true;
       }
       m_server.begin();
    }

    void output(char* data){
      if(!m_client || !m_client.connected()) {
        m_client = m_server.available();
      }
      
      if(!m_client) {
        // No client connected, nothing to do.
        return;
      }
      
      m_client.write(data);
    }

    bool readData(char* input){
      if(!m_client) {
        // No client connected, nothing to read.
        return false;
      }

      while(m_client.available() > 0) {
        m_buffer[m_buffer_pos] = m_client.read();
        if(m_buffer[m_buffer_pos] == '\n') {
          m_buffer[m_buffer_pos] = NULL;
          strcpy(input, m_buffer);
          m_buffer_pos = 0;
          return true;
        }
        ++m_buffer_pos;
        if(m_buffer_pos >= WIFI_READ_BUFFER_LEN) {
          m_buffer_pos = 0;
        }
      }
      return false;
    }
};
#endif
