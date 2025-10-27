//
// synthmode.cpp
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

#include <cstdio>
#include <cstring>

#include "synthmode.h"
#include "lcd/lcd.h"
#include "synth/synthbase.h"

namespace
{
using TInstrument    = CSynthMode::TInstrument;
using TCollection    = CSynthMode::TInstrumentCollection;

const TInstrument GeneralMIDIInstruments[] = {
        {"Acoustic Grand Piano", 0},
        {"Bright Acoustic Piano", 1},
        {"Electric Grand Piano", 2},
        {"Honky-tonk Piano", 3},
        {"Electric Piano 1", 4},
        {"Electric Piano 2", 5},
        {"Harpsichord", 6},
        {"Clavinet", 7},
        {"Celesta", 8},
        {"Glockenspiel", 9},
        {"Music Box", 10},
        {"Vibraphone", 11},
        {"Marimba", 12},
        {"Xylophone", 13},
        {"Tubular Bells", 14},
        {"Dulcimer", 15},
        {"Drawbar Organ", 16},
        {"Percussive Organ", 17},
        {"Rock Organ", 18},
        {"Church Organ", 19},
        {"Reed Organ", 20},
        {"Accordion", 21},
        {"Harmonica", 22},
        {"Tango Accordion", 23},
        {"Acoustic Guitar (nylon)", 24},
        {"Acoustic Guitar (steel)", 25},
        {"Electric Guitar (jazz)", 26},
        {"Electric Guitar (clean)", 27},
        {"Electric Guitar (muted)", 28},
        {"Overdriven Guitar", 29},
        {"Distortion Guitar", 30},
        {"Guitar Harmonics", 31},
        {"Acoustic Bass", 32},
        {"Electric Bass (finger)", 33},
        {"Electric Bass (pick)", 34},
        {"Fretless Bass", 35},
        {"Slap Bass 1", 36},
        {"Slap Bass 2", 37},
        {"Synth Bass 1", 38},
        {"Synth Bass 2", 39},
        {"Violin", 40},
        {"Viola", 41},
        {"Cello", 42},
        {"Contrabass", 43},
        {"Tremolo Strings", 44},
        {"Pizzicato Strings", 45},
        {"Orchestral Harp", 46},
        {"Timpani", 47},
        {"String Ensemble 1", 48},
        {"String Ensemble 2", 49},
        {"SynthStrings 1", 50},
        {"SynthStrings 2", 51},
        {"Choir Aahs", 52},
        {"Voice Oohs", 53},
        {"Synth Voice", 54},
        {"Orchestra Hit", 55},
        {"Trumpet", 56},
        {"Trombone", 57},
        {"Tuba", 58},
        {"Muted Trumpet", 59},
        {"French Horn", 60},
        {"Brass Section", 61},
        {"SynthBrass 1", 62},
        {"SynthBrass 2", 63},
        {"Soprano Sax", 64},
        {"Alto Sax", 65},
        {"Tenor Sax", 66},
        {"Baritone Sax", 67},
        {"Oboe", 68},
        {"English Horn", 69},
        {"Bassoon", 70},
        {"Clarinet", 71},
        {"Piccolo", 72},
        {"Flute", 73},
        {"Recorder", 74},
        {"Pan Flute", 75},
        {"Blown Bottle", 76},
        {"Shakuhachi", 77},
        {"Whistle", 78},
        {"Ocarina", 79},
        {"Lead 1 (square)", 80},
        {"Lead 2 (sawtooth)", 81},
        {"Lead 3 (calliope)", 82},
        {"Lead 4 (chiff)", 83},
        {"Lead 5 (charang)", 84},
        {"Lead 6 (voice)", 85},
        {"Lead 7 (fifths)", 86},
        {"Lead 8 (bass+lead)", 87},
        {"Pad 1 (new age)", 88},
        {"Pad 2 (warm)", 89},
        {"Pad 3 (polysynth)", 90},
        {"Pad 4 (choir)", 91},
        {"Pad 5 (bowed)", 92},
        {"Pad 6 (metallic)", 93},
        {"Pad 7 (halo)", 94},
        {"Pad 8 (sweep)", 95},
        {"FX 1 (rain)", 96},
        {"FX 2 (soundtrack)", 97},
        {"FX 3 (crystal)", 98},
        {"FX 4 (atmosphere)", 99},
        {"FX 5 (brightness)", 100},
        {"FX 6 (goblins)", 101},
        {"FX 7 (echoes)", 102},
        {"FX 8 (sci-fi)", 103},
        {"Sitar", 104},
        {"Banjo", 105},
        {"Shamisen", 106},
        {"Koto", 107},
        {"Kalimba", 108},
        {"Bag Pipe", 109},
        {"Fiddle", 110},
        {"Shanai", 111},
        {"Tinkle Bell", 112},
        {"Agogo", 113},
        {"Steel Drums", 114},
        {"Woodblock", 115},
        {"Taiko Drum", 116},
        {"Melodic Tom", 117},
        {"Synth Drum", 118},
        {"Reverse Cymbal", 119},
        {"Guitar Fret Noise", 120},
        {"Breath Noise", 121},
        {"Seashore", 122},
        {"Bird Tweet", 123},
        {"Telephone Ring", 124},
        {"Helicopter", 125},
        {"Applause", 126},
        {"Gunshot", 127},
};

const TCollection GeneralMIDICollection{GeneralMIDIInstruments, sizeof(GeneralMIDIInstruments) / sizeof(TInstrument)};
}

CSynthMode::CSynthMode()
        : m_pUI(nullptr),
          m_pCurrentSynth(nullptr),
          m_pActiveSynth(nullptr),
          m_CurrentSynthType(TSynth::MT32),
          m_ActiveSynthType(TSynth::MT32),
          m_bActive(false),
          m_nCurrentChannel(0),
          m_SynthStates{}
{
        for (size_t i = 0; i < SynthCount; ++i)
        {
                m_SynthStates[i].Collection = {nullptr, 0};
                for (u8 ch = 0; ch < ChannelCount; ++ch)
                        m_SynthStates[i].ChannelInstrumentIndex[ch] = 0;
        }
}

void CSynthMode::Initialize(CUserInterface& UI)
{
        m_pUI = &UI;
        RegisterInstrumentCollection(TSynth::MT32, GeneralMIDICollection);
        RegisterInstrumentCollection(TSynth::SoundFont, GeneralMIDICollection);
}

void CSynthMode::SetCurrentSynth(TSynth Synth, CSynthBase* pSynth)
{
        const size_t nIndex = SynthToIndex(Synth);
        m_CurrentSynthType = Synth;
        m_pCurrentSynth = pSynth;

        EnsureInstrumentIndexValid(Synth, m_nCurrentChannel);

        if (m_bActive)
        {
                if (pSynth == nullptr)
                {
                        if (m_pUI)
                                m_pUI->ClearModePresenter(this);
                        m_pActiveSynth = nullptr;
                        m_bActive = false;
                        return;
                }

                m_ActiveSynthType = Synth;
                m_pActiveSynth = pSynth;
                EnsureInstrumentIndexValid(Synth, m_nCurrentChannel);
                ApplyInstrument();
        }
        else if (m_SynthStates[nIndex].Collection.nCount == 0)
        {
                // No instrument list available; ensure indexes are zeroed
                for (u8 ch = 0; ch < ChannelCount; ++ch)
                        m_SynthStates[nIndex].ChannelInstrumentIndex[ch] = 0;
        }
}

void CSynthMode::Activate()
{
        if (m_bActive || m_pUI == nullptr)
                return;
        if (m_pCurrentSynth == nullptr)
        {
                m_pUI->ShowSystemMessage("Synth unavailable");
                return;
        }

        m_bActive = true;
        m_ActiveSynthType = m_CurrentSynthType;
        m_pActiveSynth = m_pCurrentSynth;
        EnsureInstrumentIndexValid(m_ActiveSynthType, m_nCurrentChannel);
        m_pUI->SetModePresenter(this);
        ApplyInstrument();
}

void CSynthMode::Deactivate()
{
        if (!m_bActive)
                return;

        m_bActive = false;
        m_pActiveSynth = nullptr;
        if (m_pUI)
                m_pUI->ClearModePresenter(this);
}

bool CSynthMode::HandleEncoder(s8 nDelta)
{
        if (!m_bActive || nDelta == 0)
                return false;

        const TInstrumentCollection* pCollection = GetCollection(m_ActiveSynthType);
        if (pCollection == nullptr || pCollection->nCount == 0)
                return true;

        size_t& nCurrentIndex = m_SynthStates[SynthToIndex(m_ActiveSynthType)].ChannelInstrumentIndex[m_nCurrentChannel];
        const int nCount = static_cast<int>(pCollection->nCount);
        int nNewIndex = static_cast<int>(nCurrentIndex) + static_cast<int>(nDelta);
        nNewIndex %= nCount;
        if (nNewIndex < 0)
                nNewIndex += nCount;

        if (static_cast<size_t>(nNewIndex) == nCurrentIndex)
                return true;

        nCurrentIndex = static_cast<size_t>(nNewIndex);
        ApplyInstrument();
        return true;
}

void CSynthMode::AdjustChannel(int nDelta)
{
        if (!m_bActive || nDelta == 0)
                return;

        int nNewChannel = static_cast<int>(m_nCurrentChannel) + nDelta;
        nNewChannel %= ChannelCount;
        if (nNewChannel < 0)
                nNewChannel += ChannelCount;

        if (static_cast<u8>(nNewChannel) == m_nCurrentChannel)
                return;

        m_nCurrentChannel = static_cast<u8>(nNewChannel);
        EnsureInstrumentIndexValid(m_ActiveSynthType, m_nCurrentChannel);
        ApplyInstrument();
}

size_t CSynthMode::SynthToIndex(TSynth Synth)
{
        return static_cast<size_t>(Synth);
}

void CSynthMode::RegisterInstrumentCollection(TSynth Synth, const TInstrumentCollection& Collection)
{
        const size_t nIndex = SynthToIndex(Synth);
        m_SynthStates[nIndex].Collection = Collection;

        for (u8 ch = 0; ch < ChannelCount; ++ch)
        {
                if (Collection.nCount == 0)
                        m_SynthStates[nIndex].ChannelInstrumentIndex[ch] = 0;
                else if (m_SynthStates[nIndex].ChannelInstrumentIndex[ch] >= Collection.nCount)
                        m_SynthStates[nIndex].ChannelInstrumentIndex[ch] = 0;
        }
}

const CSynthMode::TInstrumentCollection* CSynthMode::GetCollection(TSynth Synth) const
{
        const size_t nIndex = SynthToIndex(Synth);
        if (nIndex >= SynthCount)
                return nullptr;
        const TInstrumentCollection& Collection = m_SynthStates[nIndex].Collection;
        return Collection.nCount == 0 ? nullptr : &Collection;
}

void CSynthMode::EnsureInstrumentIndexValid(TSynth Synth, u8 nChannel)
{
        const size_t nIndex = SynthToIndex(Synth);
        if (nIndex >= SynthCount)
                return;

        const TInstrumentCollection& Collection = m_SynthStates[nIndex].Collection;
        size_t& nInstrumentIndex = m_SynthStates[nIndex].ChannelInstrumentIndex[nChannel % ChannelCount];
        if (Collection.nCount == 0)
        {
                nInstrumentIndex = 0;
        }
        else if (nInstrumentIndex >= Collection.nCount)
        {
                nInstrumentIndex = 0;
        }
}

const CSynthMode::TInstrument* CSynthMode::GetInstrument(TSynth Synth, u8 nChannel) const
{
        const TInstrumentCollection* pCollection = GetCollection(Synth);
        if (pCollection == nullptr)
                return nullptr;

        const size_t nIndex = SynthToIndex(Synth);
        const size_t nChannelIndex = m_SynthStates[nIndex].ChannelInstrumentIndex[nChannel % ChannelCount];
        return &pCollection->pInstruments[nChannelIndex % pCollection->nCount];
}

void CSynthMode::ApplyInstrument()
{
        if (!m_bActive || m_pActiveSynth == nullptr)
                return;

        const TInstrument* pInstrument = GetInstrument(m_ActiveSynthType, m_nCurrentChannel);
        if (pInstrument == nullptr)
                return;

        const u32 nMessage = static_cast<u32>(pInstrument->Program) << 8 | 0xC0 | m_nCurrentChannel;
        m_pActiveSynth->HandleMIDIShortMessage(nMessage);
}

void CSynthMode::Draw(CLCD& LCD)
{
        const char* pSynthName = m_ActiveSynthType == TSynth::MT32 ? "MT-32" : "SoundFont";
        const TInstrument* pInstrument = GetInstrument(m_ActiveSynthType, m_nCurrentChannel);

        char HeaderLine[32];
        char ChannelLine[32];
        char InstrumentLine[64];
        char RowLine[64];
        char HelpLine[48];

        std::snprintf(HeaderLine, sizeof(HeaderLine), "Synth: %s", pSynthName);

        if (pInstrument)
        {
                std::snprintf(ChannelLine, sizeof(ChannelLine), "Ch%02u P%03u",
                              static_cast<unsigned>(m_nCurrentChannel + 1),
                              static_cast<unsigned>(pInstrument->Program + 1));
                std::snprintf(InstrumentLine, sizeof(InstrumentLine), "%s", pInstrument->pName);
        }
        else
        {
                std::snprintf(ChannelLine, sizeof(ChannelLine), "Ch%02u",
                              static_cast<unsigned>(m_nCurrentChannel + 1));
                std::snprintf(InstrumentLine, sizeof(InstrumentLine), "No instruments available");
        }

        const u8 nWidth = LCD.Width();
        const size_t nPrefixLen = std::strlen(ChannelLine);
        if (nWidth > nPrefixLen + 1)
        {
                const size_t nAvailable = nWidth - nPrefixLen - 1;
                std::snprintf(RowLine, sizeof(RowLine), "%s %.*s", ChannelLine,
                              static_cast<int>(nAvailable), InstrumentLine);
        }
        else
        {
                std::snprintf(RowLine, sizeof(RowLine), "%s", ChannelLine);
        }

        if (nWidth >= 20)
                std::snprintf(HelpLine, sizeof(HelpLine), "Enc:Prog B2:Bank B3/4:Ch Press:Swap");
        else
                std::snprintf(HelpLine, sizeof(HelpLine), "EncP B2Bk B34Ch PrSwp");

        const u8 nHeight = LCD.Height();
        if (nHeight >= 1)
                LCD.Print(HeaderLine, 0, 0, true, false);
        if (nHeight >= 2)
                LCD.Print(RowLine, 0, 1, true, false);
        if (nHeight >= 3)
                LCD.Print(InstrumentLine, 0, 2, true, false);
        if (nHeight >= 4)
                LCD.Print(HelpLine, 0, 3, true, false);
}
