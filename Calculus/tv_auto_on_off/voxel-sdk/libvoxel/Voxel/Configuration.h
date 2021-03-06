/*
 * TI Voxel Lib component.
 *
 * Copyright (c) 2014 Texas Instruments Inc.
 */

#ifndef VOXEL_CONFIGURATION_H
#define VOXEL_CONFIGURATION_H

#include "Common.h"

namespace Voxel
{

/**
 * \ingroup Util
 */

class VOXEL_EXPORT Configuration
{
protected:
  struct _Path
  {
    String standardPath;
    String environmentVariable;
  };
  
  static const Map<String, _Path> _pathTypes;
  
  bool _getPaths(const String &type, Vector<String> &paths);
  
  bool _get(const String &type, String &name);

  static bool _addPath(const String &type, const String &path);
  
public:
  inline bool getFirmwarePaths(Vector<String> &paths) { return _getPaths("firmware", paths); }
  inline bool getConfPaths(Vector<String> &paths) { return _getPaths("conf", paths); }
  inline bool getLibPaths(Vector<String> &paths) { return _getPaths("lib", paths); }

  inline static bool addFirmwarePath(const String &path) { return _addPath("firmware", path); }
  inline static bool addConfPath(const String &path) { return _addPath("conf", path); }
  inline static bool addLibPath(const String &path) { return _addPath("lib", path); }
  
  /// Updates "name" to full path
  inline bool getConfFile(String &name) { return _get("conf", name); }
  inline bool getFirmwareFile(String &name) { return _get("firmware", name); }
  inline bool geLibFile(String &name) { return _get("lib", name); }
  
};

class VOXEL_EXPORT ConfigSet
{
public:
  Map<String, String> params;
  Vector<String> paramNames;
};

class MainConfigurationFile;

class VOXEL_EXPORT ConfigurationFile
{
protected:
  bool _get(const String &section, const String &name, String &value) const;
public:
  typedef Map<String, ConfigSet> ConfigSetMap;
  
  ConfigSetMap configs;
  
  virtual bool isPresent(const String &section, const String &name) const;
  virtual String get(const String &section, const String &name) const;
  int getInteger(const String &section, const String &name) const;
  float getFloat(const String &section, const String &name) const;
  bool getBoolean(const String &section, const String &name) const;
  virtual bool getConfigSet(const String &section, const ConfigSet *&configSet) const;
  
  virtual bool read(const String &configFile);
  
  ConfigurationFile() {}
  virtual ~ConfigurationFile() {}
  
  friend class MainConfigurationFile;
};

class VOXEL_EXPORT MainConfigurationFile: public ConfigurationFile
{
  Vector<String> _cameraProfileNames;
  Map<String, ConfigurationFile> _cameraProfiles;
  String _defaultCameraProfileName;
  String _currentCameraProfileName;
  ConfigurationFile *_currentCameraProfile;
public:
  MainConfigurationFile(): _currentCameraProfile(0) {}
  
  virtual bool read(const String &configFile);
  
  virtual String get(const String &section, const String &name) const;
  virtual bool isPresent(const String &section, const String &name) const;
  
  bool setCurrentCameraProfile(const String &profileName);
  
  bool getDefaultCameraProfile(ConfigurationFile *&defaultCameraProfile);
  bool getCameraProfile(const String &profileName, ConfigurationFile *&cameraProfile);
  
  const String &getDefaultCameraProfileName() { return _defaultCameraProfileName; }
  const String &getCurrentProfileName() { return _currentCameraProfileName; }
  const Vector<String> &getCameraProfileNames() { return _cameraProfileNames; }
  
  virtual ~MainConfigurationFile() {}
};


}

#endif // CONFIGURATION_H
