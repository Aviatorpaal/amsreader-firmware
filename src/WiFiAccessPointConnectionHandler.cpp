/**
 * @copyright Utilitech AS 2023
 * License: Fair Source
 * 
 */

#include "WiFiAccessPointConnectionHandler.h"

WiFiAccessPointConnectionHandler::WiFiAccessPointConnectionHandler(RemoteDebug* debugger) {
    this->debugger = debugger;
    this->mode = NETWORK_MODE_WIFI_AP;
}

bool WiFiAccessPointConnectionHandler::connect(NetworkConfig config, SystemConfig sys) {
		//wifi_softap_set_dhcps_offer_option(OFFER_ROUTER, 0); // Disable default gw

		WiFi.mode(WIFI_AP);
		WiFi.softAP(config.ssid, config.psk);

		dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
		dnsServer.start(53, PSTR("*"), WiFi.softAPIP());
        connected = true;

        return true;
}

void WiFiAccessPointConnectionHandler::disconnect(unsigned long reconnectDelay) {
	WiFi.disconnect(true);
	WiFi.softAPdisconnect(true);
	WiFi.enableAP(false);
	WiFi.mode(WIFI_OFF);
	yield();
}

bool WiFiAccessPointConnectionHandler::isConnected() {
    return connected;
}

#if defined(ESP32)
void WiFiAccessPointConnectionHandler::eventHandler(WiFiEvent_t event, WiFiEventInfo_t info) {
    uint8_t mac[6];
    IPAddress stationIP;
    switch(event) {
        case ARDUINO_EVENT_WIFI_AP_START:
            if(debugger->isActive(RemoteDebug::INFO)) debugger->printf_P(PSTR("WiFi access point started with SSID %s\n"), config.ssid);
            break;
	    case ARDUINO_EVENT_WIFI_AP_STOP:
            if(debugger->isActive(RemoteDebug::INFO)) debugger->printf_P(PSTR("WiFi access point stopped!\n"));
            break;
	    case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
            memcpy(mac, info.wifi_ap_staconnected.mac, 6);
            if(debugger->isActive(RemoteDebug::INFO)) debugger->printf_P(PSTR("Client connected to AP, client MAC: %02x:%02x:%02x:%02x:%02x:%02x\n"), mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            break;
	    case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
            memcpy(mac, info.wifi_ap_staconnected.mac, 6);
            if(debugger->isActive(RemoteDebug::INFO)) debugger->printf_P(PSTR("Client disconnected from AP, client MAC: %02x:%02x:%02x:%02x:%02x:%02x\n"), mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            break;
	    case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
            stationIP = info.wifi_ap_staipassigned.ip.addr;
            if(debugger->isActive(RemoteDebug::INFO)) debugger->printf_P(PSTR("Client was assigned IP %s\n"), stationIP.toString().c_str());
            break;
    }
}
#endif

bool WiFiAccessPointConnectionHandler::isConfigChanged() {
	return configChanged;
}

void WiFiAccessPointConnectionHandler::getCurrentConfig(NetworkConfig& networkConfig) {
	networkConfig = this->config;
}