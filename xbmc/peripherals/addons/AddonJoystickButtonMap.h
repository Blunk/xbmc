/*
 *      Copyright (C) 2014-2016 Team Kodi
 *      http://kodi.tv
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
 *  along with this Program; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */
#pragma once

#include "PeripheralAddon.h" // for FeatureMap
#include "addons/include/kodi_peripheral_utils.hpp"
#include "input/joysticks/DriverPrimitive.h"
#include "input/joysticks/IJoystickButtonMap.h"
#include "input/joysticks/JoystickTypes.h"
#include "peripherals/PeripheralTypes.h"

namespace PERIPHERALS
{
  class CPeripheral;

  class CAddonJoystickButtonMap : public JOYSTICK::IJoystickButtonMap
  {
  public:
    CAddonJoystickButtonMap(CPeripheral* device, const std::weak_ptr<CPeripheralAddon>& addon, const std::string& strControllerId);

    virtual ~CAddonJoystickButtonMap(void);

    // Implementation of IJoystickButtonMap
    virtual std::string ControllerID(void) const override { return m_strControllerId; }

    virtual bool Load(void) override;

    virtual void Reset(void) override;

    virtual bool GetFeature(
      const JOYSTICK::CDriverPrimitive& primitive,
      JOYSTICK::FeatureName& feature
    ) override;

    virtual JOYSTICK::FEATURE_TYPE GetFeatureType(
      const JOYSTICK::FeatureName& feature
    ) override;

    virtual bool GetScalar(
      const JOYSTICK::FeatureName& feature,
      JOYSTICK::CDriverPrimitive& primitive
    ) override;

    virtual bool AddScalar(
      const JOYSTICK::FeatureName& feature,
      const JOYSTICK::CDriverPrimitive& primitive
    ) override;

    virtual bool GetAnalogStick(
      const JOYSTICK::FeatureName& feature,
      JOYSTICK::CDriverPrimitive& up,
      JOYSTICK::CDriverPrimitive& down,
      JOYSTICK::CDriverPrimitive& right,
      JOYSTICK::CDriverPrimitive& left
    ) override;

    virtual bool AddAnalogStick(
      const JOYSTICK::FeatureName& feature,
      const JOYSTICK::CDriverPrimitive& up,
      const JOYSTICK::CDriverPrimitive& down,
      const JOYSTICK::CDriverPrimitive& right,
      const JOYSTICK::CDriverPrimitive& left
    ) override;

    virtual bool GetAccelerometer(
      const JOYSTICK::FeatureName& feature,
      JOYSTICK::CDriverPrimitive& positiveX,
      JOYSTICK::CDriverPrimitive& positiveY,
      JOYSTICK::CDriverPrimitive& positiveZ
    ) override;

    virtual bool AddAccelerometer(
      const JOYSTICK::FeatureName& feature,
      const JOYSTICK::CDriverPrimitive& positiveX,
      const JOYSTICK::CDriverPrimitive& positiveY,
      const JOYSTICK::CDriverPrimitive& positiveZ
    ) override;

  private:
    typedef std::map<JOYSTICK::CDriverPrimitive, JOYSTICK::FeatureName> DriverMap;

    // Utility functions
    static DriverMap CreateLookupTable(const FeatureMap& features);

    bool UnmapPrimitive(const JOYSTICK::CDriverPrimitive& primitive);

    static bool ResetPrimitive(ADDON::JoystickFeature& feature, const ADDON::DriverPrimitive& primitive);

    CPeripheral* const  m_device;
    std::weak_ptr<CPeripheralAddon>  m_addon;
    const std::string   m_strControllerId;
    FeatureMap          m_features;
    DriverMap           m_driverMap;
  };
}
