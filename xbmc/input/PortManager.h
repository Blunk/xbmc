/*
 *      Copyright (C) 2015-2016 Team Kodi
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

#include "peripherals/PeripheralTypes.h"
#include "threads/CriticalSection.h"
#include "utils/Observer.h"

#include <map>
#include <vector>

namespace JOYSTICK { class IJoystickInputHandler; }

namespace PERIPHERALS { class CPeripheral; }

/*!
 * \ingroup input
 *
 * \brief Class to manage ports opened by game clients
 */
class CPortManager : public Observable
{
private:
  CPortManager(void) { }

public:
  static CPortManager& Get(void);

  /*!
   * \brief Request a new port be opened with input on that port sent to the
   *        specified handler.
   *
   * \param handler  The instance accepting all input delivered to the port
   * \param port     The port number belonging to the game client
   */
  void OpenPort(JOYSTICK::IJoystickInputHandler* handler, unsigned int port,
                PERIPHERALS::PeripheralType device = PERIPHERALS::PERIPHERAL_UNKNOWN);

  /*!
   * \brief Close an opened port
   *
   * \param handler  The handler used to open the port
   */
  void ClosePort(JOYSTICK::IJoystickInputHandler* handler);

  /*!
   * \brief Map a list of devices to the available ports
   *
   * \param devices  The devices capable of providing input to the ports
   * \param portMap  The resulting map of devices to ports
   *
   * If there are more devices than open ports, multiple devices may be assigned
   * to the same port. If a device requests a specific port, this function will
   * attempt to honor that request.
   */
  void MapDevices(const std::vector<PERIPHERALS::CPeripheral*>& devices,
                  std::map<PERIPHERALS::CPeripheral*,
                  JOYSTICK::IJoystickInputHandler*>& deviceToPortMap);

private:
  struct SDevice
  {
    void* device;
    bool  bConnected;
  };

  struct SPort
  {
    JOYSTICK::IJoystickInputHandler* handler; // Input handler for this port
    unsigned int                     port;    // Port number belonging to the game client
    PERIPHERALS::PeripheralType      type;
    std::vector<SDevice>             devices;

  };

  /*!
   * \brief Functor to match ports against a specified input handler
   */
  struct PortInputHandlerEqual
  {
    PortInputHandlerEqual(JOYSTICK::IJoystickInputHandler* handler) : handler(handler) { }

    bool operator()(const SPort& port) { return port.handler == handler; }

    JOYSTICK::IJoystickInputHandler* const handler;
  };

  /*!
   * \brief Helper function to find the next open port, round-robin fashion,
   *        starting from the given port.
   *
   * A port is considered open if no other ports have a fewer number of devices
   * connected.
   */
  int GetTargetPort(int requestedPort, PERIPHERALS::PeripheralType deviceType) const;

  /*!
   * \brief Helper function to get the device's requested port
   * \param device If device is not a joystick, this returns JOYSTICK_PORT_UNKNOWN
   * \return the requested port, or JOYSTICK_PORT_UNKNOWN if no port is requested
   */
  static int GetRequestedPort(const PERIPHERALS::CPeripheral* device);

  std::vector<SPort> m_ports;
  CCriticalSection   m_mutex;
};
