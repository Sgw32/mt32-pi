//
// synthmode.h
//
// mt32-pi - A baremetal MIDI synthesizer for Raspberry Pi
// Copyright (C) 2020-2023 Dale Whinham <daleyo@gmail.com>
//
// This file is part of mt32-pi.
//
// mt32-pi is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// mt32-pi is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// mt32-pi. If not, see <http://www.gnu.org/licenses/>.
//

#ifndef _synthmode_h
#define _synthmode_h

#include <circle/types.h>

#include "lcd/ui.h"
#include "synth/synth.h"

class CSynthBase;

class CSynthMode : public CUserInterface::IModePresenter
{
public:
        struct TInstrument
        {
                const char* pName;
                u8 Program;
        };

        struct TInstrumentCollection
        {
                const TInstrument* pInstruments;
                size_t nCount;
        };

        CSynthMode();

        void Initialize(CUserInterface& UI);
        void SetCurrentSynth(TSynth Synth, CSynthBase* pSynth);

        void Activate();
        void Deactivate();
        bool IsActive() const { return m_bActive; }

        bool HandleEncoder(s8 nDelta);
        void AdjustChannel(int nDelta);

        TSynth GetActiveSynthType() const { return m_ActiveSynthType; }
        u8 GetCurrentChannel() const { return m_nCurrentChannel; }

        virtual void Draw(CLCD& LCD) override;

private:
        static constexpr size_t SynthCount   = static_cast<size_t>(TSynth::SoundFont) + 1;
        static constexpr u8 ChannelCount     = 16;

        struct TSynthState
        {
                TInstrumentCollection Collection;
                size_t ChannelInstrumentIndex[ChannelCount];
        };

        static size_t SynthToIndex(TSynth Synth);

        void RegisterInstrumentCollection(TSynth Synth, const TInstrumentCollection& Collection);
        const TInstrumentCollection* GetCollection(TSynth Synth) const;
        void EnsureInstrumentIndexValid(TSynth Synth, u8 nChannel);
        const TInstrument* GetInstrument(TSynth Synth, u8 nChannel) const;
        void ApplyInstrument();

        CUserInterface* m_pUI;
        CSynthBase* m_pCurrentSynth;
        CSynthBase* m_pActiveSynth;
        TSynth m_CurrentSynthType;
        TSynth m_ActiveSynthType;
        bool m_bActive;
        u8 m_nCurrentChannel;
        TSynthState m_SynthStates[SynthCount];
};

#endif
