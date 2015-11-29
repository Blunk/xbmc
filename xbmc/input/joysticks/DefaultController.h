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

namespace JOYSTICK
{
  class IButtonKeyHandler;
  class IButtonSequence;

  /*!
   * \brief Implementation of IJoystickInputHandler for the default controller
   *
   * \sa IJoystickInputHandler
   */
  class CDefaultController : public IJoystickInputHandler
  {
  public:
    CDefaultController(void);

    virtual ~CDefaultController(void);

    // implementation of IJoystickInputHandler
    virtual std::string ControllerID(void) const override;
    virtual InputType GetInputType(const JoystickFeature& feature) const override;
    virtual bool OnButtonPress(const JoystickFeature& feature, bool bPressed) override;
    virtual bool OnButtonMotion(const JoystickFeature& feature, float magnitude) override;
    virtual bool OnAnalogStickMotion(const JoystickFeature& feature, float x, float y) override;
    virtual bool OnAccelerometerMotion(const JoystickFeature& feature, float x, float y, float z) override;

  private:
    /*!
     * \brief Get the button key, as defined in guilib/Key.h, for the specified
     *        joystick feature/direction
     *
     * \return The button key ID
     */
    static unsigned int GetButtonKeyID(const JoystickFeature& feature, CARDINAL_DIRECTION dir = CARDINAL_DIRECTION::UNKNOWN);

    /*!
     * \brief Return a vector of the four cardinal directions
     */
    static const std::vector<CARDINAL_DIRECTION>& GetDirections(void);

    IButtonKeyHandler* const m_handler;
    IButtonSequence*         m_easterEgg;
  };
}
