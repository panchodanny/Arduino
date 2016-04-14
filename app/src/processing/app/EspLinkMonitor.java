package processing.app;

import cc.arduino.packages.BoardPort;
import cc.arduino.packages.ssh.NoInteractionUserInfo;
import cc.arduino.packages.ssh.SSHClientSetupChainRing;
import cc.arduino.packages.ssh.SSHConfigFileSetup;
import cc.arduino.packages.ssh.SSHPwdSetup;

import com.jcraft.jsch.*;

import processing.app.debug.MessageConsumer;
import processing.app.debug.MessageSiphon;

import javax.swing.*;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import static processing.app.I18n._;
import processing.app.Base;

@SuppressWarnings("serial")
public class EspLinkMonitor extends AbstractMonitor implements MessageConsumer {

  //private static final int MAX_CONNECTION_ATTEMPTS = 5;

  private final BoardPort port;
  private final String ipAddress;

  // private MessageSiphon inputConsumer;
  // private Session session;
  // private Channel channel;
  // private int connectionAttempts;

  public EspLinkMonitor(BoardPort port) {
    super(port.getLabel());
    this.port = port;
    this.ipAddress = port.getAddress();

    //TODO: localize
    Base.showMessage(_("Serial Monitor"),
                     _("This board doesn't support the Serial Monitor over the network.\n" +
                       "Please connect the board to a USB port and try again.") );

    }

  @Override
  public boolean requiresAuthorization() {
    return false;
  }

  @Override
  public String getAuthorizationKey() {
    return "runtime.pwd." + ipAddress;
  }

  @Override
  public void open() throws Exception {
  }

  @Override
  public synchronized void message(String s) {
  }

  @Override
  public void close() throws Exception {
  }

}
