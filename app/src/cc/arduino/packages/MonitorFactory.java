package cc.arduino.packages;

import processing.app.AbstractMonitor;
import processing.app.NetworkMonitor;
import processing.app.SerialMonitor;
import processing.app.EspLinkMonitor;


public class MonitorFactory {

  public AbstractMonitor newMonitor(BoardPort port) {
    if (  "network".equals(port.getProtocol() ) ) {
          if(port.getPrefs().get("distro_name") == null || "linino".equals(port.getPrefs().get("distro_name"))){
            return new NetworkMonitor(port);
          }
          if( "esplink".equals(port.getPrefs().get("distro_name") ) ){
            return new EspLinkMonitor(port);
          }


    }

    return new SerialMonitor(port);
  }

}
