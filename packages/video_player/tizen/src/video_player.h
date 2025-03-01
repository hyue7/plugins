// Copyright 2020 Samsung Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_PLUGIN_VIDEO_PLAYER_H_
#define FLUTTER_PLUGIN_VIDEO_PLAYER_H_

#include <flutter/encodable_value.h>
#include <flutter/event_channel.h>
#include <flutter/plugin_registrar.h>
#include <flutter/texture_registrar.h>
#include <player.h>

#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <string>

#include "video_player_options.h"

class VideoPlayer {
 public:
  using SeekCompletedCallback = std::function<void()>;

  VideoPlayer(flutter::PluginRegistrar *plugin_registrar,
              flutter::TextureRegistrar *texture_registrar,
              const std::string &uri, VideoPlayerOptions &options);
  ~VideoPlayer();

  int64_t GetTextureId() { return texture_id_; }

  void Play();
  void Pause();
  void SetLooping(bool is_looping);
  void SetVolume(double volume);
  void SetPlaybackSpeed(double speed);
  void SeekTo(int position,  // milliseconds
              const SeekCompletedCallback &seek_completed_cb);
  int GetPosition();  // milliseconds
  void Dispose();

 private:
  void Initialize();
  void RequestRendering();
  void SetUpEventChannel(flutter::BinaryMessenger *messenger);
  void SendInitialized();
  void OnRenderingCompleted();

  FlutterDesktopGpuSurfaceDescriptor *ObtainGpuSurface(size_t width,
                                                       size_t height);
  static void OnPrepared(void *data);
  static void OnBuffering(int percent, void *data);
  static void OnSeekCompleted(void *data);
  static void OnPlayCompleted(void *data);
  static void OnInterrupted(player_interrupted_code_e code, void *data);
  static void OnError(int code, void *data);
  static void OnVideoFrameDecoded(media_packet_h packet, void *data);
  static void ReleaseMediaPacket(void *packet);

  media_packet_h current_media_packet_ = nullptr;
  media_packet_h previous_media_packet_ = nullptr;
  bool is_initialized_ = false;
  bool is_rendering_ = false;
  player_h player_;
  std::unique_ptr<flutter::EventChannel<flutter::EncodableValue>>
      event_channel_;
  std::unique_ptr<flutter::EventSink<flutter::EncodableValue>> event_sink_;
  int64_t texture_id_;
  flutter::TextureRegistrar *texture_registrar_;
  std::unique_ptr<flutter::TextureVariant> texture_variant_;
  std::unique_ptr<FlutterDesktopGpuSurfaceDescriptor> gpu_surface_;
  std::mutex mutex_;
  SeekCompletedCallback on_seek_completed_;
  std::queue<media_packet_h> packet_queue_;
};

#endif  // FLUTTER_PLUGIN_VIDEO_PLAYER_H_
