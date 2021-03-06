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

#include "addons/IAddon.h"
#include "guilib/IWindowManagerCallback.h"
#include "peripherals/PeripheralTypes.h"
#include "peripherals/bus/PeripheralBus.h"
#include "threads/CriticalSection.h"

#include <string>

namespace PERIPHERALS
{
  class CPeripheralBusAddon : public CPeripheralBus
  {
  public:
    CPeripheralBusAddon(CPeripherals *manager);
    virtual ~CPeripheralBusAddon(void) { }

    /*!
     * \brief Get peripheral add-on by ID
     */
    bool GetAddon(const std::string &strId, ADDON::AddonPtr &addon) const;

    /*!
     * \brief Get peripheral add-on that can provide button maps for the given device
     */
    bool GetAddonWithButtonMap(const CPeripheral* device, PeripheralAddonPtr &addon) const;

    /*!
     * \brief Get the number of peripheral add-on libraries
     */
    unsigned int GetAddonCount(void) const;

    /*!
     * \brief Initialize the properties of a peripheral with a known location
     */
    bool InitializeProperties(CPeripheral* peripheral);

    // Inherited from CPeripheralBus
    virtual void         Register(CPeripheral *peripheral) override;
    virtual void         GetFeatures(std::vector<PeripheralFeature> &features) const override;
    virtual bool         HasFeature(const PeripheralFeature feature) const override;
    virtual CPeripheral* GetPeripheral(const std::string &strLocation) const override;
    virtual CPeripheral* GetByPath(const std::string &strPath) const override;
    virtual int          GetPeripheralsWithFeature(std::vector<CPeripheral *> &results, const PeripheralFeature feature) const override;
    virtual size_t       GetNumberOfPeripherals(void) const override;
    virtual size_t       GetNumberOfPeripheralsWithId(const int iVendorId, const int iProductId) const override;
    virtual void         GetDirectory(const std::string &strPath, CFileItemList &items) const override;

    bool SplitLocation(const std::string& strLocation, PeripheralAddonPtr& addon, unsigned int& peripheralIndex) const;

  protected:
    // Inherited from CPeripheralBus
    virtual bool PerformDeviceScan(PeripheralScanResults &results) override;
    virtual void UnregisterRemovedDevices(const PeripheralScanResults &results) override;
    virtual void ProcessEvents(void) override;

  private:
    void UpdateAddons(void);

    PeripheralAddonVector m_addons;
    PeripheralAddonVector m_failedAddons;
    CCriticalSection      m_critSection;
  };
}
