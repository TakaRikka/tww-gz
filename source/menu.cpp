#include "menu.h"
#include "menus/main_menu.h"
#include "menus/inventory_menu.h"
#include "menus/settings_menu.h"
#include "menus/tools_menu.h"
#include "menus/cheat_menu.h"

typedef void (*menu_render_t)();

menu_render_t l_menuDrawList[MENU_NUM] = {
    MainMenu::draw,
    CheatMenu::draw,
    InventoryMenu::draw,
    SettingsMenu::draw,
    ToolsMenu::draw,
};

menu_render_t l_currentMenu = nullptr;

void GZ_drawMenu() {
    if (l_currentMenu != nullptr) {
        l_currentMenu();
    }
}

void GZ_setMenu(int menu_idx) {
    if (menu_idx > GZ_NO_MENU) {
        l_currentMenu = l_menuDrawList[menu_idx];
    } else {
        l_currentMenu = nullptr;
    }
}

void GZ_clearMenu() {
    if (l_currentMenu) {
        l_currentMenu = nullptr;
    }
}

bool GZ_checkMenuOpen() {
    return l_currentMenu != nullptr;
}