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

#include "RetroPlayer.h"
#include "cores/VideoPlayer/DVDClock.h"
//#include "dialogs/GUIDialogOK.h"
#include "games/tags/GameInfoTag.h"
#include "input/Key.h"
#include "messaging/ApplicationMessenger.h"
#include "settings/Settings.h"
#include "threads/SingleLock.h"
#include "threads/SystemClock.h"
#include "URL.h"
#include "utils/log.h"
#include "utils/StringUtils.h"

#define PLAYSPEED_PAUSED    0
#define PLAYSPEED_NORMAL    1000
#define REWIND_SCALE        4 // rewind 4 times slower than speed of play

// Allowable framerates reported by game clients
#define MINIMUM_VALID_FRAMERATE    5
#define MAXIMUM_VALID_FRAMERATE    100

// TODO: Move to CGameClient
#define AUTOSAVE_MS   30000 // autosave every 30 seconds

#define AUDIO_FORMAT  AE_FMT_S16NE // TODO

using namespace ADDON;
using namespace GAME;

CRetroPlayer::CRetroPlayer(IPlayerCallback& callback)
  : IPlayer(callback),
    CThread("RetroPlayer"),
    m_playSpeed(PLAYSPEED_NORMAL),
    m_audioSpeedFactor(0.0),
    m_samplerate(0)
{
}

bool CRetroPlayer::OpenFile(const CFileItem& file, const CPlayerOptions& options)
{
  CLog::Log(LOGINFO, "RetroPlayer: Opening: %s", file.GetPath().c_str());

  CSingleLock lock(m_critSection);

  if (IsPlaying())
    CloseFile();

  PrintGameInfo(file);

  m_file = CFileItemPtr(new CFileItem(file));
  m_PlayerOptions = options;

  /* TODO
  // Must be called from main thread
  g_renderManager.PreInit();
  */

  Create();
  CLog::Log(LOGDEBUG, "RetroPlayer: File opened successfully");
  return true;
}

void CRetroPlayer::PrintGameInfo(const CFileItem &file) const
{
  // Get game info tag (from a mutable file item, if necessary)
  const CGameInfoTag *tag = file.GetGameInfoTag();
  CFileItem temp;
  if (!tag)
  {
    temp = file;
    if (temp.LoadGameTag())
      tag = temp.GetGameInfoTag();
  }

  // Dump discovered information to the debug log
  if (tag)
  {
    CLog::Log(LOGDEBUG, "RetroPlayer: ---------------------------------------");
    CLog::Log(LOGDEBUG, "RetroPlayer: Game tag loaded");
    CLog::Log(LOGDEBUG, "RetroPlayer: URL: %s", tag->GetURL().c_str());
    CLog::Log(LOGDEBUG, "RetroPlayer: Platform: %s", tag->GetPlatform().c_str());
    CLog::Log(LOGDEBUG, "RetroPlayer: Title: %s", tag->GetTitle().c_str());
    CLog::Log(LOGDEBUG, "RetroPlayer: Game Code: %s", tag->GetID().c_str());
    CLog::Log(LOGDEBUG, "RetroPlayer: Region: %s", tag->GetRegion().c_str());
    CLog::Log(LOGDEBUG, "RetroPlayer: Publisher: %s", tag->GetPublisher().c_str());
    CLog::Log(LOGDEBUG, "RetroPlayer: Format: %s", tag->GetFormat().c_str());
    CLog::Log(LOGDEBUG, "RetroPlayer: Cartridge Type: %s", tag->GetCartridgeType().c_str());
    CLog::Log(LOGDEBUG, "RetroPlayer: ---------------------------------------");
  }
}

bool CRetroPlayer::CloseFile(bool reopen /* = false */)
{
  CLog::Log(LOGDEBUG, "RetroPlayer: Closing file");

  CSingleLock lock(m_critSection);

  if (!m_file)
    return true; // Already closed

  m_playSpeed = PLAYSPEED_NORMAL;

  m_file.reset();

  // Set the abort request so the thread can finish up
  StopThread(false);

  m_pauseEvent.Set();

  m_audio.Stop();
  m_video.Stop();

  /* TODO
  // TODO: g_renderManager.Init() (via OpenFile()) must be called from the main
  // thread, or locking g_graphicsContext will freeze XBMC. Does g_renderManager.UnInit()
  // also need to be called from the main thread? Is CloseFile() always called
  // from the main thread?
  g_renderManager.UnInit();
  */

  CLog::Log(LOGDEBUG, "RetroPlayer: File closed");
  return true;
}

void CRetroPlayer::Process()
{
  /* TODO
  CreateAudio(m_gameClient->GetSampleRate());
  const double newFramerate = m_gameClient->GetFrameRate();

  if (m_audioSpeedFactor == 1.0)
    CLog::Log(LOGDEBUG, "RetroPlayer: Frame rate set to %f", (float)newFramerate);
  else
    CLog::Log(LOGDEBUG, "RetroPlayer: Frame rate changed from %f to %f",
      (float)(newFramerate / m_audioSpeedFactor), (float)newFramerate);

  m_video.Start(newFramerate);

  const double frametime = 1000 * 1000 / newFramerate; // microseconds

  CLog::Log(LOGDEBUG, "RetroPlayer: Beginning loop de loop");
  double nextpts = CDVDClock::GetAbsoluteClock() + frametime;
  while (!m_bStop)
  {
    if (m_playSpeed == PLAYSPEED_PAUSED)
    {
      // No need to pause audio or video, the absence of frames will pause it
      // 1s should be a good failsafe if the event isn't triggered (shouldn't happen)
      if (AbortableWait(m_pauseEvent, 1000) == WAIT_INTERRUPTED)
        break;

      nextpts = CDVDClock::GetAbsoluteClock() + frametime; // Reset the clock
      continue;
    }

    if (m_playSpeed < PLAYSPEED_PAUSED)
    {
      // Need to rewind 2 frames so that RunFrame() will update the screen
      m_gameClient->RewindFrames(2);
    }

    // Run the game client for the next frame
    m_gameClient->RunFrame();

    // Slow down (increase nextpts) if we're playing catchup after stalling
    if (nextpts < CDVDClock::GetAbsoluteClock())
      nextpts = CDVDClock::GetAbsoluteClock();

    const double realFrameTime = frametime * PLAYSPEED_NORMAL /
        (m_playSpeed > PLAYSPEED_PAUSED ? m_playSpeed : -m_playSpeed / REWIND_SCALE);

    // Slow down to 0.5x (an extra frame) if the audio is delayed
    //if (m_audio.GetDelay() * 1000 > CSettings::Get().GetInt("gamesgeneral.audiodelay"))
    //  nextpts += realFrameTime;

    CDVDClock::WaitAbsoluteClock(nextpts);
    nextpts += realFrameTime;
  }
  */
}

void CRetroPlayer::CreateAudio(double samplerate)
{
  // Default: no change in framerate
  m_audioSpeedFactor = 1.0;

  // No sound if invalid sample rate
  if (samplerate > 0)
  {
    // We want to sync the video clock to the audio. The creation of the audio
    // thread will return the sample rate decided by the audio stream.
    if (m_audio.Start(AUDIO_FORMAT, samplerate))
    {
      m_samplerate = m_audio.GetSampleRate();

      CLog::Log(LOGDEBUG, "RetroPlayer: Created audio stream with sample rate %u from reported rate of %f",
        m_samplerate, (float)samplerate);

      // If audio is playing, use that as the reference clock and adjust our framerate accordingly
      m_audioSpeedFactor = m_samplerate / samplerate;
    }
    else
    {
      CLog::Log(LOGERROR, "RetroPlayer: Error creating audio stream with sample rate %f", (float)samplerate);
    }
  }
  else
  {
    CLog::Log(LOGERROR, "RetroPlayer: Error, invalid sample rate %f, continuing without sound", (float)samplerate);
  }
}

void CRetroPlayer::Pause()
{
  if (m_playSpeed == PLAYSPEED_PAUSED)
  {
    m_playSpeed = PLAYSPEED_NORMAL;
    m_pauseEvent.Set();
  }
  else
  {
    m_playSpeed = PLAYSPEED_PAUSED;
  }
}

void CRetroPlayer::ToFFRW(int iSpeed /* = 0 */)
{
  bool unpause = false;
  if (m_playSpeed == PLAYSPEED_PAUSED && iSpeed != PLAYSPEED_PAUSED)
    unpause = true;

  m_playSpeed = iSpeed * PLAYSPEED_NORMAL;

  if (unpause)
    m_pauseEvent.Set();
}

void CRetroPlayer::Seek(bool bPlus /* = true */, bool bLargeStep /* = false */, bool bChapterOverride /* = false */)
{
  if (bPlus) // Cannot seek forward in time.
    return;

  if (!IsPlaying())
    return;

  int seek_seconds = bLargeStep ? 10 : 1; // Seem like good values, probably depends on max rewind, needs testing
  //m_gameClient->RewindFrames((unsigned int)(seek_seconds * m_gameClient->GetFrameRate())); // TODO
}

void CRetroPlayer::SeekPercentage(float fPercent)
{
  /* TODO
  if (!m_gameClient || !m_gameClient->GetMaxFrames())
    return; // Rewind not supported for game.

  if (fPercent < 0.0f)
    fPercent = 0.0f;
  else if (fPercent > 100.0f)
    fPercent = 100.0f;

  const int max_buffer     = m_gameClient->GetMaxFrames();
  const int current_buffer = m_gameClient->GetAvailableFrames();

  const int target_buffer  = (int)(max_buffer * fPercent / 100.0f);
  const int rewind_frames  = current_buffer - target_buffer;

  if (rewind_frames > 0)
    m_gameClient->RewindFrames(rewind_frames);
  */
}

float CRetroPlayer::GetPercentage()
{
  /* TODO
  if (!m_gameClient || !m_gameClient->GetMaxFrames())
    return 0.0f;

  return (100.0f * m_gameClient->GetAvailableFrames()) / m_gameClient->GetMaxFrames();
  */
  return 0.0f;
}

void CRetroPlayer::SeekTime(int64_t iTime)
{
  /* TODO
  if (!m_gameClient)
    return;

  // Avoid SIGFPE
  int64_t totalTime = GetTotalTime();
  SeekPercentage(totalTime > 0 ? 100.0f * iTime / totalTime : 0.0f);
  */
}

int64_t CRetroPlayer::GetTime()
{
  /* TODO
  if (!m_gameClient || !m_gameClient->GetFrameRate())
    return 0;

  int current_buffer = m_gameClient->GetAvailableFrames();
  return 1000 * current_buffer / m_gameClient->GetFrameRate(); // Millisecs
  */
  return 0;
}

int64_t CRetroPlayer::GetTotalTime()
{
  /* TODO
  if (!m_gameClient || !m_gameClient->GetFrameRate())
    return 0;

  int max_buffer = m_gameClient->GetMaxFrames();
  return 1000 * max_buffer / m_gameClient->GetFrameRate(); // Millisecs
  */
  return 0;
}
