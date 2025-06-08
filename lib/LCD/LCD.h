#ifndef INDUSTRIAL_LCD_UI_H
#define INDUSTRIAL_LCD_UI_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Keypad.h"

#define LCD_WIDTH 128
#define LCD_HEIGHT 64
#define MENU_VISIBLE_LINES 3

class MenuPage; // Forward
enum LCDCustomKeypad {
    LCD_KEYPAD_EVENT_OK = 0,
    LCD_KEYPAD_EVENT_LEFT,
    LCD_KEYPAD_EVENT_RIGHT,
};
// Base class for any UI page or screen
class UIPage {
public:
    virtual void onEnter() {}
    virtual void onExit() {}
    virtual void onKey(LCDCustomKeypad e) {}
    virtual void update() {}
    virtual void draw(Adafruit_SSD1306& display) = 0;
    virtual ~UIPage() {}
};

// Menu item types
enum MenuItemType { MENU_ACTION, MENU_SUBMENU, MENU_EDIT_BOOL, MENU_EDIT_INT };

// Menu item base class
class MenuItem {
public:
    String label;
    MenuItemType type;
    MenuPage* submenu;            // For submenus
    std::function<void()> action; // For actions
    int* editInt;
    int editIntMin, editIntMax;
    bool* editBool;

    MenuItem(const String& label, std::function<void()> action);
    MenuItem(const String& label, MenuPage* submenu);
    MenuItem(const String& label, int* editInt, int minVal, int maxVal);
    MenuItem(const String& label, bool* editBool);
};

// Page with selectable menu items
class MenuPage : public UIPage {
public:
    MenuPage(const String& title, std::vector<MenuItem> items);

    void onEnter() override;
    void onExit() override;
    void onKey(LCDCustomKeypad e) override;
    void update() override;
    void draw(Adafruit_SSD1306& display) override;

    int getSelectedIndex() const { return _selected; }


protected:
    String _title;
    std::vector<MenuItem> _items;
    int _selected;
    bool _editing;
    int _scrollOffset;
};

// Popup dialog for notifications, errors, etc.
class Popup : public UIPage {
public:
    Popup(const String& msg, uint32_t timeoutMs = 2000);
    void onEnter() override;
    void update() override;
    void draw(Adafruit_SSD1306& display) override;
    bool isExpired() const;
private:
    String _msg;
    uint32_t _timeoutMs;
    uint32_t _startMs;
};

class ConfirmationPopup : public UIPage {
public:
    ConfirmationPopup(const String& msg, std::function<void(bool)> cb)
        : _msg(msg), _callback(cb), _choice(false) {}

    void onEnter() override {}
    void onKey(LCDCustomKeypad k) ;
    void draw(Adafruit_SSD1306& display) ;
private:
    String _msg;
    std::function<void(bool)> _callback;
    bool _choice;
};

// Main UI controller
class IndustrialLcdUI {
public:
    IndustrialLcdUI(Adafruit_SSD1306& display, Keypad& keypad);

    bool begin();
    void loop();

    // Manage pages
    void pushPage(UIPage* page);
    void popPage();
    UIPage* currentPage();

    // Show popup
    void showPopup(const String& msg, uint32_t timeoutMs = 2000);

    // Status bar (optional)
    void setStatusBarProvider(std::function<void(Adafruit_SSD1306&)> provider);

    // Config
    void setSleepTimeout(uint32_t ms);

private:
    Adafruit_SSD1306& _display;
    Keypad& _keypad;
    std::vector<UIPage*> _pageStack;
    Popup* _popup;
    std::function<void(Adafruit_SSD1306&)> _statusBarProvider;

    uint32_t _lastInputMs;
    uint32_t _sleepTimeoutMs;
    bool _sleeping;
    const GFXfont * _font;

    void processKeypad();
    void wake();
};

#endif