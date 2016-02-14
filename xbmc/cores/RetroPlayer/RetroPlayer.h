/*
 *      Copyright (C) 2012-2015 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */
#pragma once

#include "RetroPlayerAudio.h"
#include "RetroPlayerVideo.h"
#include "cores/IPlayer.h"
#include "FileItem.h"
#include "threads/Thread.h"
#include "threads/Event.h"

#include <stdint.h>
#include <string>

class CRetroPlayer : public IPlayer, protected CThread
{
public:
  CRetroPlayer(IPlayerCallback& callback);
  virtual ~CRetroPlayer() { CloseFile(); }

  // implementation of IPlayer
  virtual bool OpenFile(const CFileItem& file, const CPlayerOptions& options) override;
  virtual bool CloseFile(bool reopen = false) override;
  virtual bool IsPlaying() const override { return !m_bStop && m_file; }
  virtual void Pause() override;
  virtual bool IsPaused() const override { return m_playSpeed == 0; }
  virtual bool HasVideo() const override { return true; }
  virtual bool HasAudio() const override { return true; }
  virtual bool HasGame() const override { return true; };
  virtual void Seek(bool bPlus = true, bool bLargeStep = false, bool bChapterOverride = false) override;
  virtual void SeekPercentage(float fPercent = 0) override;
  virtual float GetPercentage() override;
  virtual void GetAudioInfo(std::string& strAudioInfo) override { strAudioInfo = "CRetroPlayer:GetAudioInfo"; }
  virtual void GetVideoInfo(std::string& strVideoInfo) override { strVideoInfo = "CRetroPlayer:GetVideoInfo"; }
  virtual void GetGeneralInfo(std::string& strGeneralInfo) override { strGeneralInfo = "CRetroPlayer:GetGeneralInfo"; }
  virtual void SeekTime(int64_t iTime = 0) override;
  virtual int64_t GetTime() override;
  virtual int64_t GetTotalTime() override;
  virtual void ToFFRW(int iSpeed = 0) override;

  // Game API
  void VideoFrame(const uint8_t* data, unsigned int size, unsigned int width, unsigned int height, AVPixelFormat format) { m_video.VideoFrame(data, size, width, height, format); }
  void AudioFrames(const uint8_t* data, unsigned int size, unsigned int frames, AEDataFormat format) { m_audio.AudioFrames(data, size, frames, format); }

protected:
  virtual void Process();

private:
  /**
   * Dump game information (if any) to the debug log.
   */
  void PrintGameInfo(const CFileItem &file) const;

  /**
   * Create the audio component. Chooses a compatible samplerate and returns
   * a multiplier representing the framerate adjustment factor, allowing us to
   * sync the video clock to the audio.
   * @param  samplerate - the game client's reported audio sample rate
   * @return the framerate multiplier (chosen samplerate / specified samplerate)
   *         or 1.0 if no audio.
   */
  void CreateAudio(double samplerate);

  CRetroPlayerVideo    m_video;
  CRetroPlayerAudio    m_audio;

  CFileItemPtr         m_file;

  CPlayerOptions       m_PlayerOptions;
  int                  m_playSpeed; // Normal play speed is PLAYSPEED_NORMAL (1000)
  double               m_audioSpeedFactor; // Factor by which the audio is sped up
  CEvent               m_pauseEvent;
  CCriticalSection     m_critSection; // For synchronization of Open() and Close() calls

  unsigned int m_samplerate;
};
