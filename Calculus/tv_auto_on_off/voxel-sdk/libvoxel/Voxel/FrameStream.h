/*
 * TI Voxel Lib component.
 *
 * Copyright (c) 2014 Texas Instruments Inc.
 */

#ifndef VOXEL_FRAME_STREAM_H
#define VOXEL_FRAME_STREAM_H

#include <Frame.h>
#include <SerializedObject.h>

namespace Voxel
{
  
class CameraSystem;

class FrameGenerator;
  
struct VOXEL_EXPORT FrameStreamHeader
{
  char version[2]; // 0 -> major, 1 -> minor
  GeneratorIDType generatorIDs[3]; // For raw (processed), depth and point cloud, in that order
};

struct VOXEL_EXPORT FrameStreamPacket
{
  char magic[6]; // supposed to be VOXEL
  
  enum PacketType
  {
    PACKET_DATA = 0,
    PACKET_GENERATOR_CONFIG = 1
  };
  
  uint8_t type; // PacketType
  
  uint32_t size;
  
  SerializedObject object;

  FrameStreamPacket() { strcpy(magic, "VOXEL"); }
  
  bool readHeader(InputFileStream &in);
  bool read(InputFileStream &in);
  
  bool write(OutputFileStream &out);
  
  inline bool verifyMagic() { return magic[0] == 'V' && magic[1] == 'O' && magic[2] == 'X' && magic[3] == 'E' && magic[4] == 'L'; }
};

struct VOXEL_EXPORT GeneratorConfigurationSubPacket
{
  uint8_t frameType;
  uint32_t size;
  SerializedObject config;
  
  bool read(SerializedObject &object);
  bool write(SerializedObject &object);
};

class VOXEL_EXPORT FrameStreamWriter
{
  OutputFileStream &_stream;
  OutputFileStream _internalStream;
  
  Mutex _mutex;
  
  size_t _frameCount;
  FrameStreamHeader _header;
  FrameStreamPacket _rawpacket, _configPacket;
  GeneratorConfigurationSubPacket _generatorConfigSubPacket;
  
  bool _writeHeader();
  
  bool _init(GeneratorIDType processedRawFrameGeneratorID, GeneratorIDType depthFrameGeneratorID, GeneratorIDType pointCloudFrameGeneratorID);
  
public:
  FrameStreamWriter(const String &filename, GeneratorIDType processedRawFrameGeneratorID, GeneratorIDType depthFrameGeneratorID, GeneratorIDType pointCloudFrameGeneratorID);
  FrameStreamWriter(OutputFileStream &stream, GeneratorIDType processedRawFrameGeneratorID, GeneratorIDType depthFrameGeneratorID, GeneratorIDType pointCloudFrameGeneratorID);
  
  inline bool isStreamGood() { return _stream.good(); }
  
  bool write(FramePtr rawUnprocessed);
  
  inline SerializedObject &getConfigObject() { return _generatorConfigSubPacket.config; }
  bool writeGeneratorConfiguration(uint frameType);
  // Assumes the config sub-packet has been populated by using getConfigObject()
  
  bool close();
  
  virtual ~FrameStreamWriter() { close(); }
  
  
};

typedef Ptr<FrameStreamWriter> FrameStreamWriterPtr;

class VOXEL_EXPORT FrameStreamReader
{
  InputFileStream &_stream;
  InputFileStream _internalStream;
  
  Vector<FileOffsetType> _allPacketOffsets;
  
  Vector<IndexType> _dataPacketLocation, _configPacketLocation;
  
  FrameStreamHeader _header;
  
  size_t _currentPacketIndex; // index on _allPacketOffsets
  size_t _currentFrameIndex; // index on _dataPacketLocation
  
  CameraSystem &_sys;
  
  Ptr<FrameGenerator> _frameGenerator[3]; // for processed raw, depth and point cloud
  
  FrameStreamPacket _dataPacket, _configPacket;
  GeneratorConfigurationSubPacket _configSubPacket;
  
  bool _init();
  
  bool _getPacket(size_t packetIndex, FrameStreamPacket &packet);
  bool _readConfigPacket(size_t packetIndex);
  
public:
  FrameStreamReader(const String &fileName, CameraSystem &sys);
  FrameStreamReader(InputFileStream &stream, CameraSystem &sys);
  
  inline bool isStreamGood() { return _stream.good(); }
  
  Vector<FramePtr> frames; // 4 entries - raw (2 types), depth and point cloud corresponding to currently read frame index
  
  bool readNext();
  bool seekTo(size_t position);
  
  inline size_t currentPosition() { return _currentFrameIndex; }
  inline size_t size() { return _dataPacketLocation.size(); }
  
  bool close();
  
  virtual ~FrameStreamReader() {}
};

typedef Ptr<FrameStreamReader> FrameStreamReaderPtr;
  
  
}

#endif
