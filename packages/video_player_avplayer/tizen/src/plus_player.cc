// Copyright 2023 Samsung Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "plus_player.h"

#include <app_manager.h>
#include <dlfcn.h>
#include <system_info.h>
#include <unistd.h>

#include "log.h"

PlusPlayer::PlusPlayer(flutter::BinaryMessenger *messenger, void *native_window,
                       std::string video_format)
    : VideoPlayer(messenger),
      native_window_(native_window),
      video_format_(video_format) {}

PlusPlayer::~PlusPlayer() { Dispose(); }

int64_t PlusPlayer::Create(const std::string &uri, int drm_type,
                           const std::string &license_server_url,
                           bool is_prebuffer_mode) {
  LOG_INFO("[PlusPlayer] Create player.");
  if (!video_format_.compare("dash")) {
    player_ = plusplayer::PlusPlayer::Create(plusplayer::PlayerType::kDASH);
  } else {
    player_ = plusplayer::PlusPlayer::Create();
  }

  if (!player_) {
    LOG_ERROR("[PlusPlayer] Fail to create player.");
    return -1;
  }

  if (!player_->Open(uri)) {
    LOG_ERROR("[PlusPlayer] Fail to open uri :  %s.", uri.c_str());
    return -1;
  }

  char *appId = nullptr;
  int ret = app_manager_get_app_id(getpid(), &appId);
  if (ret != APP_MANAGER_ERROR_NONE) {
    LOG_ERROR("[PlusPlayer] app_manager_get_app_id failed : %s.",
              get_error_message(ret));
    return -1;
  }
  player_->SetAppId(std::string(appId));
  free(appId);

  player_->RegisterListener(this);

  if (drm_type != 0) {
    if (!SetDrm(uri, drm_type, license_server_url)) {
      LOG_ERROR("[PlusPlayer] Failed to set drm.");
      return -1;
    }
  }

  if (!SetDisplay()) {
    LOG_ERROR("[PlusPlayer] Failed to set display.");
    return -1;
  }

  SetDisplayRoi(0, 0, 1, 1);

  if (is_prebuffer_mode) {
    player_->SetPrebufferMode(true);
    is_prebuffer_mode_ = true;
  }

  if (!player_->PrepareAsync()) {
    LOG_ERROR("[PlusPlayer] Failed to prepare");
    return -1;
  }
  return SetUpEventChannel();
}

void PlusPlayer::Dispose() {
  LOG_INFO("[PlusPlayer] Disposing.");

  if (player_) {
    player_->Stop();
    player_->Close();
  }

  // drm should be released after destroy of player
  if (drm_manager_) {
    drm_manager_->ReleaseDrmSession();
  }
}

void PlusPlayer::SetDisplayRoi(int32_t x, int32_t y, int32_t width,
                               int32_t height) {
  plusplayer::Geometry roi;
  roi.x = x;
  roi.y = y;
  roi.w = width;
  roi.h = height;
  if (!player_->SetDisplayRoi(roi)) {
    LOG_ERROR("[PlusPlayer] SetDisplayRoi failed.");
  }
}

void PlusPlayer::Play() {
  LOG_INFO("[PlusPlayer] Play video.");
  plusplayer::State state = player_->GetState();
  if (state < plusplayer::State::kTrackSourceReady) {
    LOG_ERROR("[PlusPlayer] Player is not ready.");
    return;
  }

  if (state <= plusplayer::State::kReady) {
    if (!player_->Start()) {
      LOG_ERROR("[PlusPlayer] Fail to start.");
    }
  } else if (state == plusplayer::State::kPaused) {
    if (!player_->Resume()) {
      LOG_ERROR("[PlusPlayer] Fail to resume playing.");
    }
  }
}

bool PlusPlayer::SetActivate() {
  if (!player_->Activate(plusplayer::kTrackTypeVideo)) {
    LOG_ERROR("[PlusPlayer] Fail to activate video.");
    return false;
  }
  if (!player_->Activate(plusplayer::kTrackTypeAudio)) {
    LOG_ERROR("[PlusPlayer] Fail to activate audio.");
    return false;
  }
  if (!player_->Activate(plusplayer::kTrackTypeSubtitle)) {
    LOG_ERROR("[PlusPlayer] Fail to activate subtitle.");
  }

  return true;
}

bool PlusPlayer::SetDeactivate() {
  if (is_prebuffer_mode_) {
    player_->Stop();
    return true;
  }

  if (!player_->Deactivate(plusplayer::kTrackTypeVideo)) {
    LOG_ERROR("[PlusPlayer] Fail to activate video.");
    return false;
  }
  if (!player_->Deactivate(plusplayer::kTrackTypeAudio)) {
    LOG_ERROR("[PlusPlayer] Fail to activate audio.");
    return false;
  }
  if (!player_->Deactivate(plusplayer::kTrackTypeSubtitle)) {
    LOG_ERROR("[PlusPlayer] Fail to activate subtitle.");
  }

  return true;
}

void PlusPlayer::Pause() {
  LOG_INFO("[PlusPlayer] Pause video.");
  plusplayer::State state = player_->GetState();
  if (state < plusplayer::State::kReady) {
    LOG_ERROR("[PlusPlayer] Player is not ready.");
    return;
  }

  if (state == plusplayer::State::kPlaying) {
    if (!player_->Pause()) {
      LOG_ERROR("[PlusPlayer] Fail to pause video.");
    }
  }
}

void PlusPlayer::SetLooping(bool is_looping) {
  LOG_ERROR("[PlusPlayer] Not support to set looping.");
}

void PlusPlayer::SetVolume(double volume) {
  if (player_->GetState() == plusplayer::State::kPlaying ||
      player_->GetState() == plusplayer::State::kPaused) {
    player_->SetVolume(volume);
  }
}

void PlusPlayer::SetPlaybackSpeed(double speed) {
  LOG_INFO("[PlusPlayer] Sets playback speed(%f).", speed);
  if (player_->GetState() <= plusplayer::State::kIdle) {
    LOG_ERROR("[PlusPlayer] Player is not prepared.");
    return;
  }
  if (!player_->SetPlaybackRate(speed)) {
    LOG_ERROR("[PlusPlayer] Fail to set playback rate.");
  }
}

void PlusPlayer::SeekTo(int32_t position, SeekCompletedCallback callback) {
  LOG_INFO("PlusPlayer seeks to position(%d)", position);
  if (player_->GetState() < plusplayer::State::kReady) {
    LOG_ERROR("[PlusPlayer] Player is not ready.");
    return;
  }

  if (on_seek_completed_) {
    LOG_ERROR("[PlusPlayer] Player is already seeking.");
    return;
  }

  on_seek_completed_ = std::move(callback);
  if (!player_->Seek(position)) {
    on_seek_completed_ = nullptr;
    LOG_ERROR("[PlusPlayer] Fail to seek.");
  }
}

int32_t PlusPlayer::GetPosition() {
  uint64_t position = 0;
  plusplayer::State state = player_->GetState();
  if (state == plusplayer::State::kPlaying ||
      state == plusplayer::State::kPaused) {
    if (!player_->GetPlayingTime(&position)) {
      LOG_ERROR("[PlusPlayer] Fail to get the current playing time.");
    }
  }
  return position;
}

int32_t PlusPlayer::GetDuration() {
  int64_t duration = 0;
  if (player_->GetState() >= plusplayer::State::kTrackSourceReady) {
    if (!player_->GetDuration(&duration)) {
      LOG_ERROR("[PlusPlayer] Fail to get the duration.");
    }
    LOG_INFO("[PlusPlayer] Video duration: %llu.", duration);
  }
  return duration;
}

void PlusPlayer::GetVideoSize(int32_t *width, int32_t *height) {
  if (player_->GetState() >= plusplayer::State::kTrackSourceReady) {
    bool found = false;
    std::vector<plusplayer::Track> tracks = player_->GetActiveTrackInfo();
    for (auto track : tracks) {
      if (track.type == plusplayer::TrackType::kTrackTypeVideo) {
        *width = track.width;
        *height = track.height;
        found = true;
      }
    }
    if (!found) {
      LOG_ERROR("[PlusPlayer] Failed to get video size.");
    } else {
      LOG_INFO("[PlusPlayer] Video widht: %d, height: %d.", *width, *height);
    }
  }
}

bool PlusPlayer::isReady() {
  return plusplayer::State::kReady == player_->GetState();
}

bool PlusPlayer::SetDisplay() {
  int x = 0, y = 0, width = 0, height = 0;
  ecore_wl2_window_proxy_->ecore_wl2_window_geometry_get(native_window_, &x, &y,
                                                         &width, &height);
  int surface_id =
      ecore_wl2_window_proxy_->ecore_wl2_window_surface_id_get(native_window_);
  if (surface_id < 0) {
    LOG_ERROR("[PlusPlayer] Failed to get surface id.");
    return false;
  }
  bool ret = player_->SetDisplay(plusplayer::DisplayType::kOverlay, surface_id,
                                 x, y, width, height);
  if (!ret) {
    LOG_ERROR("[PlusPlayer] Failed to set display.");
    return false;
  }

  ret = player_->SetDisplayMode(plusplayer::DisplayMode::kDstRoi);
  if (!ret) {
    LOG_ERROR("[PlusPlayer] Failed to set display mode.");
    return false;
  }

  return true;
}

flutter::EncodableList PlusPlayer::getTrackInfo(int32_t track_type) {
  if (!player_) {
    LOG_ERROR("[PlusPlayer] Player not created.");
    return {};
  }

  plusplayer::State state = player_->GetState();
  if (state == plusplayer::State::kNone || state == plusplayer::State::kIdle) {
    LOG_ERROR("[PlusPlayer] Player is in invalid state.");
    return {};
  }

  plusplayer::TrackType type;
  switch (track_type) {
    case 1:
      type = plusplayer::TrackType::kTrackTypeAudio;
      break;
    case 2:
      type = plusplayer::TrackType::kTrackTypeVideo;
      break;
    case 3:
      type = plusplayer::TrackType::kTrackTypeSubtitle;
      break;
    default:
      LOG_ERROR("[PlusPlayer] Invalid track type: %d", track_type);
      return {};
  }

  int track_count = player_->GetTrackCount(type);
  if (track_count <= 0) {
    return {};
  }

  const std::vector<plusplayer::Track> track_info = player_->GetTrackInfo();
  if (track_info.empty()) {
    return {};
  }

  flutter::EncodableList trackSelections = {};
  flutter::EncodableMap trackSelection = {};
  trackSelection.insert(
      {flutter::EncodableValue("trackType"), flutter::EncodableValue(type)});
  if (type == plusplayer::TrackType::kTrackTypeVideo) {
    LOG_INFO("[PlusPlayer] Video track count: %d", track_count);
    for (const auto &track : track_info) {
      if (track.type == plusplayer::kTrackTypeVideo) {
        trackSelection.insert_or_assign(flutter::EncodableValue("trackId"),
                                        flutter::EncodableValue(track.index));
        trackSelection.insert_or_assign(flutter::EncodableValue("width"),
                                        flutter::EncodableValue(track.width));
        trackSelection.insert_or_assign(flutter::EncodableValue("height"),
                                        flutter::EncodableValue(track.height));
        trackSelection.insert_or_assign(flutter::EncodableValue("bitrate"),
                                        flutter::EncodableValue(track.bitrate));
        LOG_INFO(
            "[PlusPlayer] video track info[%d]: width[%d], height[%d], "
            "bitrate[%d]",
            track.index, track.width, track.height, track.bitrate);

        trackSelections.push_back(flutter::EncodableValue(trackSelection));
      }
    }
  } else if (type == plusplayer::TrackType::kTrackTypeAudio) {
    LOG_INFO("[PlusPlayer] Audio track count: %d", track_count);
    for (const auto &track : track_info) {
      if (track.type == plusplayer::kTrackTypeAudio) {
        trackSelection.insert_or_assign(flutter::EncodableValue("trackId"),
                                        flutter::EncodableValue(track.index));
        trackSelection.insert_or_assign(
            flutter::EncodableValue("language"),
            flutter::EncodableValue(track.language_code));
        trackSelection.insert_or_assign(
            flutter::EncodableValue("channel"),
            flutter::EncodableValue(track.channels));
        trackSelection.insert_or_assign(flutter::EncodableValue("bitrate"),
                                        flutter::EncodableValue(track.bitrate));
        LOG_INFO(
            "[PlusPlayer] audio track info[%d]: language[%s], channel[%d], "
            "sample_rate[%d], bitrate[%d]",
            track.index, track.language_code.c_str(), track.channels,
            track.sample_rate, track.bitrate);

        trackSelections.push_back(flutter::EncodableValue(trackSelection));
      }
    }
  } else if (type == plusplayer::TrackType::kTrackTypeSubtitle) {
    LOG_INFO("[PlusPlayer] Subtitle track count: %d", track_count);
    for (const auto &track : track_info) {
      if (track.type == plusplayer::kTrackTypeSubtitle) {
        trackSelection.insert_or_assign(flutter::EncodableValue("trackId"),
                                        flutter::EncodableValue(track.index));
        trackSelection.insert_or_assign(
            flutter::EncodableValue("language"),
            flutter::EncodableValue(track.language_code));
        LOG_INFO("[PlusPlayer] subtitle track info[%d]: language[%s]",
                 track.index, track.language_code.c_str());

        trackSelections.push_back(flutter::EncodableValue(trackSelection));
      }
    }
  }

  return trackSelections;
}

bool PlusPlayer::SetTrackSelection(int32_t track_id, int32_t track_type) {
  LOG_INFO("[PlusPlayer] track_id: %d,track_type: %d", track_id, track_type);

  if (!player_) {
    LOG_ERROR("[PlusPlayer] Player not created.");
    return false;
  }

  plusplayer::State state = player_->GetState();
  if (state == plusplayer::State::kNone || state == plusplayer::State::kIdle) {
    LOG_ERROR("[PlusPlayer] Player is in invalid state.");
    return false;
  }

  plusplayer::TrackType type;
  switch (track_type) {
    case 1:
      type = plusplayer::TrackType::kTrackTypeAudio;
      break;
    case 2:
      type = plusplayer::TrackType::kTrackTypeVideo;
      break;
    case 3:
      type = plusplayer::TrackType::kTrackTypeSubtitle;
      break;
    default:
      LOG_ERROR("[PlusPlayer] Invalid track type: %d", track_type);
      return {};
  }
  if (!player_->SelectTrack(type, track_id)) {
    LOG_ERROR("[PlusPlayer] Player select track failed.");
    return false;
  }
  return true;
}

bool PlusPlayer::SetDrm(const std::string &uri, int drm_type,
                        const std::string &license_server_url) {
  drm_manager_ = std::make_unique<DrmManager>();
  if (!drm_manager_->CreateDrmSession(drm_type, true)) {
    LOG_ERROR("[PlusPlayer] Failed to create drm session.");
    return false;
  }

  int drm_handle = 0;
  if (!drm_manager_->GetDrmHandle(&drm_handle)) {
    LOG_ERROR("[PlusPlayer] Failed to get drm handle.");
    return false;
  }

  plusplayer::drm::Type type;
  switch (drm_type) {
    case DrmManager::DrmType::DRM_TYPE_PLAYREADAY:
      type = plusplayer::drm::Type::kPlayready;
      break;
    case DrmManager::DrmType::DRM_TYPE_WIDEVINECDM:
      type = plusplayer::drm::Type::kWidevineCdm;
      break;
    default:
      type = plusplayer::drm::Type::kNone;
      break;
  }

  plusplayer::drm::Property property;
  property.handle = drm_handle;
  property.type = type;
  property.license_acquired_cb =
      reinterpret_cast<plusplayer::drm::LicenseAcquiredCb>(OnLicenseAcquired);
  property.license_acquired_userdata =
      reinterpret_cast<plusplayer::drm::UserData>(this);
  property.external_decryption = false;
  player_->SetDrm(property);

  if (license_server_url.empty()) {
    bool success = drm_manager_->SetChallenge(
        uri, [this](const void *challenge, unsigned long challenge_len,
                    void **response, unsigned long *response_len) {
          OnLicenseChallenge(challenge, challenge_len, response, response_len);
        });
    if (!success) {
      LOG_ERROR("[PlusPlayer]Failed to set challenge.");
      return false;
    }
  } else {
    if (!drm_manager_->SetChallenge(uri, license_server_url)) {
      LOG_ERROR("[PlusPlayer]Failed to set challenge.");
      return false;
    }
  }
  return true;
}

bool PlusPlayer::OnLicenseAcquired(int *drm_handle, unsigned int length,
                                   unsigned char *pssh_data, void *user_data) {
  LOG_INFO("[PlusPlayer] License acquired.");

  PlusPlayer *self = static_cast<PlusPlayer *>(user_data);
  if (self->drm_manager_) {
    return self->drm_manager_->SecurityInitCompleteCB(drm_handle, length,
                                                      pssh_data, nullptr);
  }
  return false;
}

void PlusPlayer::OnPrepareDone(bool ret, UserData userdata) {
  LOG_DEBUG("[PlusPlayer] Prepare done, result: %d.", ret);

  if (!is_initialized_ && ret) {
    SendInitialized();
  }
}

void PlusPlayer::OnBufferStatus(const int percent, UserData userdata) {
  LOG_INFO("[PlusPlayer] Buffering percent: %d.", percent);

  if (percent == 100) {
    SendBufferingEnd();
    is_buffering_ = false;
  } else if (!is_buffering_ && percent <= 5) {
    SendBufferingStart();
    is_buffering_ = true;
  } else {
    SendBufferingUpdate(percent);
  }
}

void PlusPlayer::OnSeekDone(UserData userdata) {
  LOG_INFO("[PlusPlayer] Seek completed.");

  if (on_seek_completed_) {
    on_seek_completed_();
    on_seek_completed_ = nullptr;
  }
}

void PlusPlayer::OnEos(UserData userdata) {
  LOG_INFO("[PlusPlayer] Play completed.");
  SendPlayCompleted();
}

void PlusPlayer::OnSubtitleData(std::unique_ptr<char[]> data, const int size,
                                const plusplayer::SubtitleType &type,
                                const uint64_t duration,
                                plusplayer::SubtitleAttrListPtr attr_list,
                                UserData userdata) {
  LOG_INFO("[PlusPlayer] Subtitle updated, duration: %llu, text: %s", duration,
           data.get());
  SendSubtitleUpdate(duration, data.get());
}

void PlusPlayer::OnResourceConflicted(UserData userdata) {
  LOG_ERROR("[PlusPlayer] Resource conflicted.");
  SendError("PlusPlayer error", "Resource conflicted");
}

void PlusPlayer::OnError(const plusplayer::ErrorType &error_code,
                         UserData userdata) {
  LOG_ERROR("[PlusPlayer] Error code: %d", error_code);
  SendError("[PlusPlayer] OnError", "");
}

void PlusPlayer::OnErrorMsg(const plusplayer::ErrorType &error_code,
                            const char *error_msg, UserData userdata) {
  LOG_ERROR("[PlusPlayer] Error code: %d, message: %s.", error_code, error_msg);
  SendError("PlusPlayer error", error_msg);
}

void PlusPlayer::OnDrmInitData(int *drmhandle, unsigned int len,
                               unsigned char *psshdata,
                               plusplayer::TrackType type, UserData userdata) {
  LOG_INFO("[PlusPlayer] Drm init completed");

  if (drm_manager_) {
    if (drm_manager_->SecurityInitCompleteCB(drmhandle, len, psshdata,
                                             nullptr)) {
      player_->DrmLicenseAcquiredDone(type);
    }
  }
}

void PlusPlayer::OnAdaptiveStreamingControlEvent(
    const plusplayer::StreamingMessageType &type,
    const plusplayer::MessageParam &msg, UserData userdata) {
  LOG_INFO("[PlusPlayer] Message type: %d, is DrmInitData (%d)", type,
           type == plusplayer::StreamingMessageType::kDrmInitData);

  if (type == plusplayer::StreamingMessageType::kDrmInitData) {
    if (msg.data.empty() || 0 == msg.size) {
      LOG_ERROR("[PlusPlayer] Empty message");
      return;
    }

    if (drm_manager_) {
      drm_manager_->UpdatePsshData(msg.data.data(), msg.size);
    }
  }
}

void PlusPlayer::OnClosedCaptionData(std::unique_ptr<char[]> data,
                                     const int size, UserData userdata) {}

void PlusPlayer::OnCueEvent(const char *CueData, UserData userdata) {}

void PlusPlayer::OnDateRangeEvent(const char *DateRangeData,
                                  UserData userdata) {}

void PlusPlayer::OnStopReachEvent(bool StopReach, UserData userdata) {}

void PlusPlayer::OnCueOutContEvent(const char *CueOutContData,
                                   UserData userdata) {}

void PlusPlayer::OnChangeSourceDone(bool ret, UserData userdata) {}

void PlusPlayer::OnStateChangedToPlaying(UserData userdata) {}
