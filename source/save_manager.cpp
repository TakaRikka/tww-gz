#include <cstdio>
#include "fs.h"
#include "libtww/dolphin/os/OSCache.h"
#include "menus/settings_menu.h"
//#include "gz_flags.h"
//#include "practice.h"
#include "memfiles.h"
#include "save_manager.h"
#include "menu.h"
#include "utils/card.h"
#include "practice.h"
#include "libtww/d/com/d_com_inf_game.h"
#include "libtww/f_op/f_op_scene_req.h"

#include "color.h"
#include "libtww/MSL_C/string.h"
#include "flags.h"

static char l_filename[80];
SaveManager gSaveManager;

bool SaveManager::s_injectSave = false;
bool SaveManager::s_injectMemfile = false;

void SaveManager::injectSave(void* buffer) { // this function is loading save data with wrong alignment?
    memcpy(&g_dComIfG_gameInfo, buffer, sizeof(dSv_save_c));
    dComIfGs_getSave(g_dComIfG_gameInfo.info.mDan.mStageNo);
}

void SaveManager::injectMemfile(void* buffer) {
    memcpy(&g_dComIfG_gameInfo, buffer, sizeof(dSv_info_c));
    dComIfGs_getSave(g_dComIfG_gameInfo.info.mDan.mStageNo);
}

void SaveManager::injectDefault_before() {
    //g_dComIfG_gameInfo.info.mRestart.mLastSpeedF = 0.0f;
    g_dComIfG_gameInfo.play.mNextStage.wipe = 0;
    //g_dComIfG_gameInfo.info.mRestart.mLastMode = 0;
}

void SaveManager::injectDefault_during() {}

void SaveManager::injectDefault_after() {}

void SaveManager::defaultLoad() {
    gSaveManager.mPracticeFileOpts.inject_options_during_load = SaveManager::injectDefault_during;
    gSaveManager.mPracticeFileOpts.inject_options_after_load = SaveManager::injectDefault_after;
    GZ_clearMenu();
}

void SaveManager::loadSave(uint32_t id, const char* category, special i_specials[], int size) {
    SaveManager::injectDefault_before();

    // Load the corresponding file path and properties
    snprintf(l_filename, sizeof(l_filename), "twwgz/save_files/%s.bin", category);
    loadFile(l_filename, &gSaveManager.mPracticeSaveInfo, sizeof(gSaveManager.mPracticeSaveInfo),
             id * sizeof(gSaveManager.mPracticeSaveInfo));
    
    //Font::GZ_drawStr(l_filename, 20.f, 200.f, ColorPalette::WHITE, g_dropShadows);

    //char name[64];
    //tww_sprintf(name, "name: %s", gSaveManager.mPracticeSaveInfo.filename);
    //Font::GZ_drawStr(name, 20.f, 230.f, ColorPalette::WHITE, g_dropShadows);
    
    snprintf(l_filename, sizeof(l_filename), "twwgz/save_files/%s/%s.bin", category,
             gSaveManager.mPracticeSaveInfo.filename);

    // 0xFF is used to identify a call from file reload, which doesn't need to run the default load
    if (size != 0xFF) {
        SaveManager::defaultLoad();
    } else {
        size = last_special_size;
    }

    if (gSaveManager.mPracticeSaveInfo.requirements) {
        gSaveManager.mPracticeFileOpts.inject_options_after_load =
            gSaveManager.mPracticeSaveInfo.requirements & REQ_CAM ? setPositionCamera : setLinkInfo;
        gSaveManager.mPracticeFileOpts.inject_options_after_counter =
            gSaveManager.mPracticeSaveInfo.counter;
    }

    // If the selected file was a special, run the special callbacks
    if (i_specials) {
        for (int i = 0; i < size; ++i) {
            if (id == i_specials[i].idx) {
                if (i_specials[i].CallbackDuring) {
                    gSaveManager.mPracticeFileOpts.inject_options_during_load =
                        i_specials[i].CallbackDuring;
                }
                if (i_specials[i].CallbackAfter) {
                    gSaveManager.mPracticeFileOpts.inject_options_after_load =
                        i_specials[i].CallbackAfter;
                }
                break;
            }
        }
    }

    // Store all the info in case file reload is used
    last_save_index = id;
    strncpy(last_category, category, sizeof(last_category));
    last_special_ptr = i_specials;
    last_special_size = size;
}

void SaveManager::loadSavefile(const char* l_filename) {
    loadFile(l_filename, MEMFILE_BUF, sizeof(dSv_save_c), 0);
}

void SaveManager::triggerLoad(uint32_t id, const char* category, special i_specials[],
                                        int size) {
    loadSave(id, category, i_specials, size);

    SaveManager::loadSavefile(l_filename);
    dSv_save_c* save = (dSv_save_c*) MEMFILE_BUF;

    int state = tww_getLayerNo(save->getPlayer().mPlayerReturnPlace.mName,
                              save->getPlayer().mPlayerReturnPlace.mRoomNo, 0xFF);

    g_dComIfG_gameInfo.play.mNextStage.mRoomNo = save->getPlayer().mPlayerReturnPlace.mRoomNo;
    g_dComIfG_gameInfo.play.mNextStage.mPoint = save->getPlayer().mPlayerReturnPlace.mPlayerStatus;
    strcpy(g_dComIfG_gameInfo.play.mNextStage.mStage, save->getPlayer().mPlayerReturnPlace.mName);
    g_dComIfG_gameInfo.play.mNextStage.mLayer = state;

    // this is not setting the flag correctly
    //u8 demo23_flag_setting = save->getEvent().getFlag(DEMO23);
    //g_dComIfG_gameInfo.info.getSavedata().getEvent().setFlag(DEMO23, 1/*demo23_flag_setting*/);

    // inject options after initial stage set since some options change stage loc
    if (gSaveManager.mPracticeFileOpts.inject_options_during_load) {
        gSaveManager.mPracticeFileOpts.inject_options_during_load(); // , 1
    }

    // for debugging
    char return_name[64];
    tww_sprintf(return_name, "return name: %s", save->getPlayer().mPlayerReturnPlace.mName);
    Font::GZ_drawStr(return_name, 20.f, 230.f, ColorPalette::WHITE, g_dropShadows);

    char return_room_no[32];
    tww_sprintf(return_room_no, "return room_no: %d", save->getPlayer().mPlayerReturnPlace.mRoomNo);
    Font::GZ_drawStr(return_room_no, 20.f, 250.f, ColorPalette::WHITE, g_dropShadows);

    char player_status[32];
    tww_sprintf(player_status, "player status: %u", save->getPlayer().mPlayerReturnPlace.mPlayerStatus);
    Font::GZ_drawStr(player_status, 20.f, 270.f, ColorPalette::WHITE, g_dropShadows);

    char layer[32];
    tww_sprintf(layer, "layer: %d", state);
    Font::GZ_drawStr(layer, 20.f, 290.f, ColorPalette::WHITE, g_dropShadows);
    // debugging ended

    g_dComIfG_gameInfo.play.mNextStage.enabled = true;
    s_injectSave = true;
}

void SaveManager::triggerMemfileLoad() {
    // GZ_readMemfile already puts the savedata in g_tmpBuf
    dSv_save_c* save = (dSv_save_c*) MEMFILE_BUF;

    int state = tww_getLayerNo(save->getPlayer().mPlayerReturnPlace.mName,
                              save->getPlayer().mPlayerReturnPlace.mRoomNo, 0xFF);

    g_dComIfG_gameInfo.play.mNextStage.mRoomNo = save->getPlayer().mPlayerReturnPlace.mRoomNo;
    g_dComIfG_gameInfo.play.mNextStage.mPoint = save->getPlayer().mPlayerReturnPlace.mPlayerStatus;
    strcpy(g_dComIfG_gameInfo.play.mNextStage.mStage, save->getPlayer().mPlayerReturnPlace.mName);
    g_dComIfG_gameInfo.play.mNextStage.mLayer = state;

    gSaveManager.mPracticeFileOpts.inject_options_after_load = GZ_setLinkPosition;

    g_dComIfG_gameInfo.play.mNextStage.enabled = true;
    s_injectMemfile = true;
}

// runs at the beginning of phase_1 of dScnPly_c load sequence
void SaveManager::loadData() {
    if (s_injectMemfile) {
        SaveManager::injectMemfile(MEMFILE_BUF);
    } else {
        SaveManager::injectSave(MEMFILE_BUF); // 0, 16
    }

    GZ_activate(HAS_SEEN_HELMAROC_ARRIVING_AT_OUTSET); // 1
    GZ_activate(WATCHED_FF2_HELMAROC_CUTSCENE); // 17

/*
    // from TP - swap equip logic
    if (g_swap_equips_flag) {
        uint8_t tmp = dComIfGs_getSelectItemIndex(SELECT_ITEM_X);
        uint8_t tmp_mix = dComIfGs_getMixItemIndex(SELECT_ITEM_X);

        dComIfGs_setSelectItemIndex(SELECT_ITEM_X, dComIfGs_getSelectItemIndex(SELECT_ITEM_Y));
        dComIfGs_setSelectItemIndex(SELECT_ITEM_Y, tmp);
        dComIfGs_setMixItemIndex(SELECT_ITEM_X, dComIfGs_getMixItemIndex(SELECT_ITEM_Y));
        dComIfGs_setMixItemIndex(SELECT_ITEM_Y, tmp_mix);
    }
*/
}

void SaveManager::setLinkInfo() {
    if (dComIfGp_getPlayer()) {
        dComIfGp_getPlayer()->mCollisionRot.sy = gSaveManager.mPracticeSaveInfo.angle;
        cXyz tmp(gSaveManager.mPracticeSaveInfo.position.x,
                 gSaveManager.mPracticeSaveInfo.position.y,
                 gSaveManager.mPracticeSaveInfo.position.z);
        dComIfGp_getPlayer()->mCurrent.mPosition = tmp;
    }
}

void SaveManager::setPositionCamera() {
    if (dComIfGp_getPlayer()) {
        dComIfGp_getPlayer()->mCollisionRot.sy = gSaveManager.mPracticeSaveInfo.angle;
        cXyz tmp(gSaveManager.mPracticeSaveInfo.position.x,
                 gSaveManager.mPracticeSaveInfo.position.y,
                 gSaveManager.mPracticeSaveInfo.position.z);
        dComIfGp_getPlayer()->mCurrent.mPosition = tmp;
    }
}