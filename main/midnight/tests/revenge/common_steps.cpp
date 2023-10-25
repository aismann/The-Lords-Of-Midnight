//
//  common_steps.cpp
//  revenge
//
//  Created by Chris Wild on 25/03/2023.
//
// "No, Morkin, Corleth was testing your spirit: it is better to know your
// comrade’s mettle before the real battle begins, is it not?"
//

#include "common_steps.h"
#include "../../src/tme/scenarios/ddr/scenario_ddr_internal.h"

using namespace std;

inline ddr_character* GetDDRCharacter(LPCSTR name)
{
    return static_cast<tme::ddr_character*>(GetCharacter(name));
}


void DDRStep::GameWon()
{
    GetCharacter(DDRStep::ch_shareth)->Cmd_Dead();
    TMEStep::NightFalls();
}

void DDRStep::GameLost()
{
    GetCharacter(TMEStep::ch_luxor)->Cmd_Dead();
    TMEStep::NightFalls();
}

void DDRStep::LordKilledByCharacter(LPCSTR name, LPCSTR killedby)
{
    auto lord = GetDDRCharacter(name);
    lord->Cmd_Dead();
    lord->fighting_against = GetDDRCharacter(killedby);
}

void DDRStep::MorkinRescued()
{
    auto morkin = GetCharacter(TMEStep::ch_morkin);
    auto tarithel = GetCharacter(ch_tarithel);
    morkin->Recruited(tarithel);
}

void DDRStep::LordAtGateOfVarenorn(LPCSTR name)
{
    mxgridref gate_varenorn = mxgridref(6,92);
    auto lord = GetCharacter(name);
    lord->Location(gate_varenorn);
}

void DDRStep::LordNotAtGateOfVarenorn(LPCSTR name)
{
    mxgridref loc = mxgridref(0,0);
    auto lord = GetCharacter(name);
    lord->Location(loc);
}


void DDRStep::LordsAtGateOfVarenorn(vector<string> names)
{
    for (string &name : names) {
        LordAtGateOfVarenorn(name.c_str());
    }
}

void DDRStep::LordsNotAtGateOfVarenorn(vector<string> names)
{
    for (string &name : names) {
        LordNotAtGateOfVarenorn(name.c_str());
    }
}

