#pragma once
/*
 *      Copyright (C) 2013 Team XBMC
 *      http://www.xbmc.org
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

#include "addons/Addon.h"
#include "filesystem/IDirectory.h"

namespace XFILE
{
  class CContentAddonDirectory : public IDirectory
  {
  public:
    CContentAddonDirectory(void) {}
    virtual ~CContentAddonDirectory(void) {}

    virtual bool GetDirectory(const CURL& url, CFileItemList& items);
    virtual bool IsAllowed(const std::string& strFile) const { return true; }
    virtual bool Exists(const char* strPath);

    static bool GetAddons(ADDON::VECADDONS& addons);
    static bool GetAddons(const std::string& content, ADDON::VECADDONS& addons);

    static bool HasInstallableAddons(void);

  private:
    static bool IsRoot(const CURL& url);
    static bool IsAddon(const CURL& url);
    static bool IsMediaSource(const CURL& url) { return false; } // TODO
    static bool GetAddons(CFileItemList& items);
  };
}