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
#pragma once

#include "input/joysticks/IJoystickInputHandler.h"
#include "input/joysticks/JoystickTypes.h"

#include <vector>

#define DEFAULT_CONTROLLER_ID    "game.controller.default"

namespace JOYSTICK
{
  class IKeymapHandler;
  class IButtonSequence;
}

namespace GAME
{
  /*!
   * \brief Implementation of IJoystickInputHandler for the default controller
   *
   * \sa IJoystickInputHandler
   */
  class CDefaultController : public JOYSTICK::IJoystickInputHandler
  {
  public:
    CDefaultController(void);

    virtual ~CDefaultController(void);

    // implementation of IJoystickInputHandler
    virtual std::string ControllerID(void) const override;
    virtual JOYSTICK::INPUT_TYPE GetInputType(const JOYSTICK::FeatureName& feature) const override;
    virtual bool OnButtonPress(const JOYSTICK::FeatureName& feature, bool bPressed) override;
    virtual bool OnButtonMotion(const JOYSTICK::FeatureName& feature, float magnitude) override;
    virtual bool OnAnalogStickMotion(const JOYSTICK::FeatureName& feature, float x, float y) override;
    virtual bool OnAccelerometerMotion(const JOYSTICK::FeatureName& feature, float x, float y, float z) override;

  private:
    /*!
     * \brief Get the keymap key, as defined in Key.h, for the specified
     *        joystick feature/direction
     *
     * \param           feature The name of the feature on the default controller
     * \param[optional] dir     The direction (used for analog sticks)
     *
     * \return The key ID, or 0 if unknown
     */
    static unsigned int GetKeyID(const JOYSTICK::FeatureName& feature, JOYSTICK::CARDINAL_DIRECTION dir = JOYSTICK::CARDINAL_DIRECTION::UNKNOWN);

    /*!
     * \brief Return a vector of the four cardinal directions
     */
    static const std::vector<JOYSTICK::CARDINAL_DIRECTION>& GetDirections(void);

    JOYSTICK::IKeymapHandler* const  m_handler;
    JOYSTICK::IButtonSequence*       m_easterEgg;
  };
}
