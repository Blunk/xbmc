/*
 *      Copyright (C) 2014-2015 Team XBMC
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

#include "GUIControllerWindow.h"
#include "GUIControllerList.h"
#include "GUIFeatureList.h"
#include "GUIGameDefines.h"
#include "addons/GUIWindowAddonBrowser.h"
#include "addons/IAddon.h"
#include "guilib/GUIMessage.h"
#include "guilib/WindowIDs.h"
//#include "input/Key.h"

// To check for button mapping support
#include "dialogs/GUIDialogOK.h"
#include "peripherals/bus/virtual/PeripheralBusAddon.h"
#include "peripherals/Peripherals.h"
#include "utils/log.h"

using namespace GAME;
using namespace PERIPHERALS;

CGUIControllerWindow::CGUIControllerWindow(void) :
  CGUIWindow(WINDOW_GAME_CONTROLLERS, "GameControllers.xml"),
  m_controllerList(nullptr),
  m_featureList(nullptr)
{
}

CGUIControllerWindow::~CGUIControllerWindow(void)
{
  delete m_controllerList;
  delete m_featureList;
}

bool CGUIControllerWindow::OnMessage(CGUIMessage& message)
{
  switch (message.GetMessage())
  {
    case GUI_MSG_CLICKED:
    {
      int controlId = message.GetSenderId();

      if (controlId == CONTROL_GET_MORE)
      {
        GetMoreControllers();
        return true;
      }
      else if (CONTROL_FEATURE_BUTTONS_START <= controlId && controlId < CONTROL_FEATURE_BUTTONS_END)
      {
        OnFeatureSelected(controlId - CONTROL_FEATURE_BUTTONS_START);
        return true;
      }
      break;
    }
    case GUI_MSG_FOCUSED:
    {
      int controlId = message.GetSenderId();

      if (CONTROL_CONTROLLER_BUTTONS_START <= controlId && controlId < CONTROL_CONTROLLER_BUTTONS_END)
      {
        OnControllerFocused(controlId - CONTROL_CONTROLLER_BUTTONS_START);
        return true;
      }
      else if (CONTROL_FEATURE_BUTTONS_START <= controlId && controlId < CONTROL_FEATURE_BUTTONS_END)
      {
        OnFeatureFocused(controlId - CONTROL_FEATURE_BUTTONS_START);
        return true;
      }
    }
  }

  return CGUIWindow::OnMessage(message);
}

void CGUIControllerWindow::OnInitWindow(void)
{
  CGUIWindow::OnInitWindow();

  if (!m_featureList)
  {
    m_featureList = new CGUIFeatureList(this);
    if (!m_featureList->Initialize())
    {
      delete m_featureList;
      m_featureList = nullptr;
    }
  }

  if (!m_controllerList)
  {
    m_controllerList = new CGUIControllerList(this, m_featureList);
    if (!m_controllerList->Initialize())
    {
      delete m_controllerList;
      m_controllerList = nullptr;
    }
  }

  // Check for button mapping support (TODO: remove this)
  CPeripheralBusAddon* bus = static_cast<CPeripheralBusAddon*>(g_peripherals.GetBusByType(PERIPHERAL_BUS_ADDON));
  if (bus && bus->GetAddonCount() == 0)
  {
    // TODO: Move the XML implementation of button map storage from add-on to
    // Kodi while keeping support for add-on button-mapping

    CLog::Log(LOGERROR, "Joystick support not found");

    // "Joystick support not found"
    // "Controller configuration is disabled. Install the proper joystick support add-on."
    CGUIDialogOK::ShowAndGetInput(35011, 35012);
  }
}

void CGUIControllerWindow::OnDeinitWindow(int nextWindowID)
{
  if (m_controllerList)
  {
    m_controllerList->Deinitialize();
    delete m_controllerList;
    m_controllerList = nullptr;
  }

  if (m_featureList)
  {
    m_featureList->Deinitialize();
    delete m_featureList;
    m_featureList = nullptr;
  }

  CGUIWindow::OnDeinitWindow(nextWindowID);
}

void CGUIControllerWindow::FocusFeature(unsigned int featureIndex)
{
  // TODO: App messenger
  CGUIMessage msg(GUI_MSG_SETFOCUS, GetID(), CONTROL_FEATURE_BUTTONS_START + featureIndex);
  OnMessage(msg);
}

void CGUIControllerWindow::SetLabel(unsigned int featureIndex, const std::string& strLabel)
{
  // TODO: App messenger
  CGUIMessage msg(GUI_MSG_LABEL_SET, GetID(), CONTROL_FEATURE_BUTTONS_START + featureIndex);
  msg.SetLabel(strLabel);
  OnMessage(msg);
}

void CGUIControllerWindow::FocusController(unsigned int controllerIndex)
{
  // TODO: App messenger
  CGUIMessage msg(GUI_MSG_SETFOCUS, GetID(), CONTROL_CONTROLLER_BUTTONS_START + controllerIndex);
  OnMessage(msg);
}

void CGUIControllerWindow::OnControllerFocused(unsigned int controllerIndex)
{
  if (m_controllerList)
    m_controllerList->OnFocus(controllerIndex);
}

void CGUIControllerWindow::OnFeatureFocused(unsigned int featureIndex)
{
  if (m_featureList)
    m_featureList->OnFocus(featureIndex);
}

void CGUIControllerWindow::OnFeatureSelected(unsigned int featureIndex)
{
  if (m_featureList)
    m_featureList->OnFocus(featureIndex);
}

void CGUIControllerWindow::GetMoreControllers(void)
{
  std::string strAddonId;
  CGUIWindowAddonBrowser::SelectAddonID(ADDON::ADDON_GAME_CONTROLLER, strAddonId, false, true, false, true, false);
}
