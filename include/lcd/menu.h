//
// menu.h
//
// Simple settings menu for mt32-pi
//

#ifndef _menu_h
#define _menu_h

#include "config.h"

class CLCD;
class CUserInterface;

class CSettingsMenu
{
public:
        enum class TItem
        {
                Verbose,
                Visualization,
                SaveExit,
                Count
        };

        CSettingsMenu(CConfig& Config, CUserInterface& UI);

        void ToggleActive();
        bool IsActive() const { return m_bActive; }
        void Next();
        void Prev();
        void Adjust(int nDelta);
        void Draw(CLCD& LCD) const;

private:
        CConfig& m_Config;
        CUserInterface& m_UI;
        TItem m_CurrentItem;
        bool m_bActive;
};

#endif
