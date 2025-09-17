//
// menu.cpp
//
// Simple settings menu implementation
//

#include <cstdio>

#include "lcd/menu.h"
#include "lcd/ui.h"
#include "lcd/lcd.h"

CSettingsMenu::CSettingsMenu(CConfig& Config, CUserInterface& UI)
        : m_Config(Config),
          m_UI(UI),
          m_CurrentItem(TItem::Verbose),
          m_bActive(false)
{
}

void CSettingsMenu::ToggleActive()
{
        m_bActive = !m_bActive;
        if (!m_bActive)
                m_Config.Save("mt32-pi.cfg");
        else
                m_CurrentItem = TItem::Verbose;
}

void CSettingsMenu::Next()
{
        if (!m_bActive)
                return;
        m_CurrentItem = static_cast<TItem>((static_cast<int>(m_CurrentItem) + 1) % static_cast<int>(TItem::Count));
}

void CSettingsMenu::Prev()
{
        if (!m_bActive)
                return;
        m_CurrentItem = static_cast<TItem>((static_cast<int>(m_CurrentItem) + static_cast<int>(TItem::Count) - 1) % static_cast<int>(TItem::Count));
}

void CSettingsMenu::Adjust(int nDelta)
{
        if (!m_bActive || nDelta == 0)
                return;

        switch (m_CurrentItem)
        {
                case TItem::Verbose:
                        m_Config.SystemVerbose = !m_Config.SystemVerbose;
                        break;

                case TItem::Visualization:
                {
                        auto Mode = m_UI.GetVisualizationMode();
                        m_UI.SetVisualizationMode(Mode == CUserInterface::TVisualizationMode::ChannelLevels ?
                                                   CUserInterface::TVisualizationMode::CPULoad :
                                                   CUserInterface::TVisualizationMode::ChannelLevels);
                        break;
                }

                case TItem::SaveExit:
                        m_Config.Save("mt32-pi.cfg");
                        m_bActive = false;
                        break;

                case TItem::Count:
                        break;
        }
}

void CSettingsMenu::Draw(CLCD& LCD) const
{
        if (!m_bActive)
                return;

        char Buffer[32];
        switch (m_CurrentItem)
        {
                case TItem::Verbose:
                        snprintf(Buffer, sizeof(Buffer), "Verbose: %s", m_Config.SystemVerbose ? "on" : "off");
                        break;

                case TItem::Visualization:
                        snprintf(Buffer, sizeof(Buffer), "Viz: %s", m_UI.GetVisualizationMode() == CUserInterface::TVisualizationMode::ChannelLevels ? "levels" : "cpu");
                        break;

                case TItem::SaveExit:
                        snprintf(Buffer, sizeof(Buffer), "Save & exit");
                        break;

                case TItem::Count:
                        Buffer[0] = '\0';
                        break;
        }

        LCD.Print(Buffer, 0, 0, true, false);
}

