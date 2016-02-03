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

#include "GameSettings.h"
#include "guilib/GUIWindowManager.h"
#include "guilib/WindowIDs.h"
#include "peripherals/Peripherals.h"
#include "settings/lib/Setting.h"
#include "utils/StringUtils.h"

#include <cstring>
#include <string>
#include <vector>

using namespace GAME;

#define SETTING_PREFIX  "gamesinput.emulatedcontroller"

CGameSettings& CGameSettings::Get()
{
  static CGameSettings gameSettingsInstance;
  return gameSettingsInstance;
}

void CGameSettings::OnSettingChanged(const CSetting* setting)
{
  if (setting == NULL)
    return;

  const std::string& settingId = setting->GetId();
  if (settingId == "gamesinput.emulatedcontrollers")
  {
    PERIPHERALS::g_peripherals.TriggerDeviceScan(PERIPHERALS::PERIPHERAL_BUS_APPLICATION);
  }
}

void CGameSettings::OnSettingAction(const CSetting* setting)
{
  if (setting == NULL)
    return;

  const std::string& settingId = setting->GetId();
  if (settingId == "gamesinput.controllerconfig")
  {
    g_windowManager.ActivateWindow(WINDOW_DIALOG_GAME_CONTROLLERS);
  }
  else if (StringUtils::StartsWith(settingId, SETTING_PREFIX))
  {
    std::string strControllerIndex = settingId.substr(std::strlen(SETTING_PREFIX));
    g_windowManager.ActivateWindow(WINDOW_DIALOG_GAME_CONTROLLERS, strControllerIndex);
  }
}
