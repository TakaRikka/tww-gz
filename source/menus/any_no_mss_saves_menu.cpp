#include "menus/any_no_mss_saves_menu.h"
#include "save_specials.h"
#include "save_manager.h"

#define LINE_NUM 18
Cursor AnyNoMssSavesMenu::cursor;

Line lines[LINE_NUM] = {
    {"after intro", AFTER_INTRO, "just after the intro, on top of watchtower"},
    {"orca", ORCA, "inside orca\'s house before sword tutorial"},
    {"forest of fairies", FOREST_OF_FAIRIES, "inside forest of fairies, about to rescue tetra"},
    {"ropes 1", ROPES_1, "inside pirate ship, about to play ropes 1 minigame"},
    {"forsaken fortress 1", FF1, "at start of ff1, before gossip stone cutscene"},
    {"windfall", WINDFALL, "at windfall island, just finished talking to korl"},
    {"bombs swim", BOMBS_SWIM, "at dragon roost island, just got the wind waker"},
    {"ropes 2", ROPES_2, "inside pirate ship, about to play ropes 2 minigame"},
    {"forest haven swim", FH_SWIM, "at windfall island, about to swim to forest haven"},
    {"deku tree cutscene skip", DTCS, "at forest haven, about to do dtcs"},
    {"quiver swim", QUIVER_SWIM, "at forest haven, about to swim to thorned fairy island"},
    {"ff2 swim", FF2_SWIM, "at thorned fairy island, about to swim to ff2"},
    {"helmaroc skip", HELMAROC_SKIP, "inside ff2 tower, about to rescue aryll"},
    {"barrier skip", BARRIER_SKIP, "outside hyrule castle, about to barrier skip"},
    {"trials skip", TRIALS_SKIP, "inside ganon's tower, about to trials skip"},
    {"puppet ganon fight", PG_FIGHT, "inside puppet ganon room, about to fight puppet ganon"},
    {"morth hover", MORTH_HOVER, "inside puppet ganon room after fight, pot lid already open"},
    {"ganondorf", GANONDORF, "on top of ganon\'s tower, about to fight ganondorf"},
};

void AnyNoMssSavesMenu::draw() {
    cursor.setMode(Cursor::MODE_LIST);
    cursor.move(0, LINE_NUM);

    if (GZ_getButtonTrig(GZPad::B)) {
        GZ_setMenu(GZ_PRACTICE_MENU);
        return;
    }

    if (GZ_getButtonTrig(GZPad::A)) {
        special AnyNoMssSpecials[] = {
            special(AFTER_INTRO, nullptr, nullptr),
            special(ORCA, nullptr, nullptr),
            special(FOREST_OF_FAIRIES, SaveMngSpecial_ForestOfFairies_FirstVisit, nullptr),
            special(ROPES_1, SaveMngSpecial_Ropes1, nullptr),
            special(FF1, SaveMngSpecial_FF1_PirateShip_Night, nullptr),
            special(WINDFALL, SaveMngSpecial_Windfall_Day0, nullptr),
            special(BOMBS_SWIM, nullptr, nullptr),
            special(ROPES_2, SaveMngSpecial_Ropes2, nullptr),
            special(FH_SWIM, SaveMngSpecial_Windfall_Day0, nullptr),
            special(DTCS, SaveMngSpecial_DTCS, nullptr),
            special(QUIVER_SWIM, nullptr, nullptr),
            special(FF2_SWIM, SaveMngSpecial_ExitThornedFairy, nullptr),
            special(HELMAROC_SKIP, SaveMngSpecial_HelmarocKing, nullptr),
            special(BARRIER_SKIP, SaveMngSpecial_Hyrule2_BarrierSkip, nullptr),
            special(TRIALS_SKIP, SaveMngSpecial_TrialsSkip, nullptr),
            special(PG_FIGHT, SaveMngSpecial_PuppetGanon, nullptr),
            special(MORTH_HOVER, SaveMngSpecial_MorthHover_during, SaveMngSpecial_MorthHover_after),
            special(GANONDORF, SaveMngSpecial_Ganondorf, nullptr),
        };

        SaveManager::triggerLoad(cursor.y, "any_no_mss", AnyNoMssSpecials, sizeof(AnyNoMssSpecials) / sizeof(AnyNoMssSpecials[0]));
        GZ_clearMenu();
    }

    GZ_drawMenuLines(lines, cursor.y, LINE_NUM);
}
