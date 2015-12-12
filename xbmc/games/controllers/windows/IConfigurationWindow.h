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

#include "games/controllers/ControllerTypes.h"

#include <string>

namespace GAME
{
  /*!
   * \brief Controller configuration utility
   *
   * The utility presents a list of controllers. Also on the screen is a list of
   * features belonging to that controller.
   *
   * The configuration utility reacts to several events:
   *
   *   1) When a controller is focused, the feature list is populated with the
   *      controller's features.
   *
   *   2) When a feature is selected, the user is prompted for controller input.
   *      When finished, focus moves on to the next feature and another prompt
   *      occurs.
   */
  class IConfigurationWindow
  {
  public:
    virtual ~IConfigurationWindow(void) { }

    /*!
     * \brief Focus the specified controller
     * \param controllerIndex The index of the controller in the list of controllers
     */
    virtual void FocusController(unsigned int controllerIndex) = 0;

    /*!
     * \brief Focus the specified feature
     * \param featureIndex The index of the feature in the list of features
     */
    virtual void FocusFeature(unsigned int featureIndex) = 0;

    /*!
     * \brief Set the label of an item in the feature list
     * \param featureIndex The index of the feature in the list of features
     * \param strLabel the desired label
     */
    virtual void SetLabel(unsigned int featureIndex, const std::string& strLabel) = 0;
  };

  class IConfigurationUtility
  {
  public:
    virtual ~IConfigurationUtility(void) { }

    /*!
     * \brief  Initialize the resource
     * \return true if the resource is initialized and can be used
     *         false if the resource failed to initialize and must not be used
     */
    virtual bool Initialize(void) = 0;

    /*!
     * \brief  Deinitialize the resource
     * \remark This must be called if Initialize() returned true
     */
    virtual void Deinitialize(void) = 0;

    /*
     * \brief  Focus has been set to the specified controller
     * \param  controllerIndex The index of the controller being focused
     */
    virtual void OnControllerFocused(unsigned int controllerIndex) = 0;

    /*!
     * \brief  The specified controller has been selected
     * \param  controllerIndex The index of the controller being selected
     */
    virtual void OnControllerSelected(unsigned int featureIndex) = 0;

    /*!
     * \brief  Focus has been set to the specified feature
     * \param  featureIndex The index of the feature being focused
     */
    virtual void OnFeatureFocused(unsigned int featureIndex) = 0;

    /*!
     * \brief  The specified feature has been selected
     * \param  featureIndex The index of the feature being selected
     */
    virtual void OnFeatureSelected(unsigned int featureIndex) = 0;
  };

  /*!
   * \brief A list populated by installed controllers
   */
  class IControllerList
  {
  public:
    virtual ~IControllerList(void) { }

    /*!
     * \brief  Initialize the resource
     * \return true if the resource is initialized and can be used
     *         false if the resource failed to initialize and must not be used
     */
    virtual bool Initialize(void) = 0;

    /*!
     * \brief  Deinitialize the resource
     * \remark This must be called if Initialize() returned true
     */
    virtual void Deinitialize(void) = 0;

    /*!
     * \brief Refresh the contents of the list
     */
    virtual void Refresh(void) = 0;

    /*!
     * \brief The specified controller is being focused
     * \param controllerIndex The index of the controller in the list of controllers
     */
    virtual void OnFocus(unsigned int controllerIndex) = 0;

    virtual void OnSelect(unsigned int controllerIndex) = 0;
  };

  /*!
   * \brief A list populated by the controller's features
   */
  class IFeatureList
  {
  public:
    virtual ~IFeatureList(void) { }

    /*!
     * \brief  Initialize the resource
     * \return true if the resource is initialized and can be used
     *         false if the resource failed to initialize and must not be used
     */
    virtual bool Initialize(void) = 0;

    /*!
     * \brief  Deinitialize the resource
     * \remark This must be called if Initialize() returned true
     */
    virtual void Deinitialize(void) = 0;

    /*!
     * \brief Load the features for the specified controller
     * \param controller The controller whose features are being listed
     */
    virtual void Load(const ControllerPtr& controller) = 0;

    virtual void OnFocus(unsigned int index) = 0;

    virtual void OnSelect(unsigned int index) = 0;

    virtual void OnUnfocus(void) = 0;

    virtual bool PromptForInput(unsigned int featureIndex) = 0;

    virtual void AbortPrompt(void) = 0;
  };

  /*!
   * \brief IFeatureButton
   */
  class IFeatureButton
  {
  public:
    virtual ~IFeatureButton(void) { }

    /*!
     * \brief Prompt the user for input to map to the button's feature
     * \return true input was mapped to the button's feature, false if the prompt was aborted
     */
    virtual bool PromptForInput(void) = 0;

    /*!
     * \brief Abort an input prompt
     * \return True if aborted, false if button wasn't prompting for input
     */
    virtual void Abort(void) = 0;
  };

  class IConfigurationWizard
  {
  public:
    virtual ~IConfigurationWizard(void) { }

    virtual void Run(unsigned int featureIndex) = 0;

    virtual bool Abort(void) = 0;
  };
}
