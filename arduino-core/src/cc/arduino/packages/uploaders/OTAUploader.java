/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  BasicUploader - generic command line uploader implementation
  Part of the Arduino project - http://www.arduino.cc/

  Copyright (c) 2004-05
  Hernando Barragan
  Copyright (c) 2012
  Cristian Maglie <c.maglie@arduino.cc>
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
/*

  OTAUploader - Over The Air Uploader is part of Arduino
  Copyright (c) 2016
  Sergio Tomasello <sergio@arduino.org>

 */


package cc.arduino.packages.uploaders;

import static processing.app.I18n._;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import processing.app.BaseNoGui;
import processing.app.I18n;
import processing.app.PreferencesData;
import processing.app.Serial;
import processing.app.SerialException;
import processing.app.debug.RunnerException;
import processing.app.debug.TargetPlatform;
import processing.app.helpers.OSUtils;
import processing.app.helpers.PreferencesMap;
import processing.app.helpers.StringReplacer;
import cc.arduino.packages.Uploader;
import cc.arduino.packages.BoardPort;

public class OTAUploader extends Uploader {

private final BoardPort port;

  public OTAUploader(BoardPort port)
  {
    this.port = port;
  }

  // public boolean requiresAuthorization() {
  //   return (this.port.getPrefs().get("via_ota.auth") != null) ? this.port.getPrefs().get("via_ota.auth").contentEquals("yes") : false;
  // }

  // @Override
  // public String getAuthorizationKey() {
  //   return "runtime.pwd." + this.port.getAddress();
  // }

  public boolean uploadUsingPreferences(File sourcePath, String buildPath, String className, boolean usingProgrammer, List<String> warningsAccumulator) throws Exception {
    // FIXME: Preferences should be reorganized
    TargetPlatform targetPlatform = BaseNoGui.getTargetPlatform();
    PreferencesMap prefs = PreferencesData.getMap();
    prefs.putAll(BaseNoGui.getBoardPreferences());
    
    String tool = prefs.getOrExcept("upload.via_ota.tool");
    if (tool.contains(":")) {
      String[] split = tool.split(":", 2);
      targetPlatform = BaseNoGui.getCurrentTargetPlatformFromPackage(split[0]);
      tool = split[1];
    }
    prefs.putAll(targetPlatform.getTool(tool));

    if (verbose)
      prefs.put("upload.verbose", prefs.getOrExcept("upload.params.verbose"));
    else
      prefs.put("upload.verbose", prefs.getOrExcept("upload.params.quiet"));

    prefs.put("board.ipaddress", port.getAddress());

    prefs.put("build.path", buildPath);
    prefs.put("build.project_name", className);
    if (verbose)
      prefs.put("upload.verbose", prefs.getOrExcept("upload.params.verbose"));
    else
      prefs.put("upload.verbose", prefs.getOrExcept("upload.params.quiet"));

    boolean uploadResult;
    try {
      String pattern = prefs.getOrExcept("upload.pattern");
      String[] cmd = StringReplacer.formatAndSplit(pattern, prefs, true);
      uploadResult = executeUploadCommand(cmd);
    } catch (RunnerException e) {
      throw e;
    } catch (Exception e) {
      throw new RunnerException(e);
    }

    return uploadResult;
  }


  public boolean burnBootloader() throws Exception {
      return false;
   }
}
