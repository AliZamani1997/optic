#include "LCD.h"
#include "font.h"
#include "font2.h"
#include "Fonts/FreeSans9pt7b.h"

const GFXfont *LCD_Font = &scripts_Inter_28pt_SemiBold7pt7b;
// const GFXfont * LCD_Font = &LCD_Font_16;

// ---------- MenuItem -------------
MenuItem::MenuItem(const String &label, std::function<void()> action)
    : label(label), type(MENU_ACTION), submenu(nullptr), action(action), editInt(nullptr), editBool(nullptr) {}

MenuItem::MenuItem(const String &label, MenuPage *submenu)
    : label(label), type(MENU_SUBMENU), submenu(submenu), action(nullptr), editInt(nullptr), editBool(nullptr) {}

MenuItem::MenuItem(const String &label, int *editInt, int minVal, int maxVal)
    : label(label), type(MENU_EDIT_INT), submenu(nullptr), action(nullptr),
      editInt(editInt), editIntMin(minVal), editIntMax(maxVal), editBool(nullptr) {}

MenuItem::MenuItem(const String &label, bool *editBool)
    : label(label), type(MENU_EDIT_BOOL), submenu(nullptr), action(nullptr),
      editInt(nullptr), editBool(editBool) {}

// ------------ MenuPage ---------------
MenuPage::MenuPage(UI *ui, const String &title, std::vector<MenuItem> items)
    : _title(title), _items(items), _selected(0), _editing(false), _scrollOffset(0), _ui(ui) {}

void MenuPage::onEnter() { _editing = false; }
void MenuPage::onExit() { _editing = false; }

void MenuPage::onKey(LCDCustomKeypad k)
{
    int n = _items.size();
    if (_editing)
    {
        // Edit mode for fields
        auto &item = _items[_selected];
        if (item.type == MENU_EDIT_INT)
        {
            if (k == LCD_KEYPAD_EVENT_LEFT)
            {
                if (*(item.editInt) > item.editIntMin)
                    (*(item.editInt))--;
            }
            else if (k == LCD_KEYPAD_EVENT_RIGHT)
            {
                if (*(item.editInt) < item.editIntMax)
                    (*(item.editInt))++;
            }
            else if (k == LCD_KEYPAD_EVENT_OK)
            {
                _editing = false;
            }
        }
        else if (item.type == MENU_EDIT_BOOL)
        {
            if (k == LCD_KEYPAD_EVENT_OK || k == LCD_KEYPAD_EVENT_LEFT || k == LCD_KEYPAD_EVENT_RIGHT)
            {
                *(item.editBool) = !(*(item.editBool));
                _editing = false;
            }
        }
        return;
    }
    // Normal menu navigation
    if (k == LCD_KEYPAD_EVENT_LEFT)
    {
        _selected = (_selected + n - 1) % n;
    }
    else if (k == LCD_KEYPAD_EVENT_RIGHT)
    {
        _selected = (_selected + 1) % n;
    }
    else if (k == LCD_KEYPAD_EVENT_OK)
    {
        auto &item = _items[_selected];
        if (item.type == MENU_ACTION && item.action)
            item.action();
        if (item.type == MENU_SUBMENU && item.submenu)
        {
            _ui->pushPage(this);
            // _pageStack.back()->onKey(LCD_KEYPAD_EVENT_OK);
        }
        if (item.type == MENU_EDIT_INT || item.type == MENU_EDIT_BOOL)
            _editing = true;
    }

    // Scroll offset logic:
    if (_selected < _scrollOffset)
        _scrollOffset = _selected;
    else if (_selected >= _scrollOffset + MENU_VISIBLE_LINES)
        _scrollOffset = _selected - MENU_VISIBLE_LINES + 1;
}

void MenuPage::update() {}
void MenuPage::draw(Adafruit_SSD1306 &display)
{
    display.clearDisplay();
    display.drawLine(0, 0, LCD_WIDTH - 1, 0, SSD1306_WHITE);
    display.drawLine(0, 0, 0, LCD_HEIGHT - 1, SSD1306_WHITE);
    display.drawLine(0, LCD_HEIGHT - 1, LCD_WIDTH - 1, LCD_HEIGHT - 1, SSD1306_WHITE);
    display.drawLine(LCD_WIDTH - 1, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1, SSD1306_WHITE);

    display.setTextSize(1);
    display.setFont(nullptr);
    display.setTextColor(SSD1306_WHITE);
    int16_t tx, ty;
    uint16_t tw, th;
    display.getTextBounds(_title, (int16_t)0, (int16_t)0, &tx, &ty, &tw, &th);
    display.drawLine(0, th + 2, LCD_WIDTH - 1, th + 2, SSD1306_WHITE);
    display.setCursor((LCD_WIDTH - tw) / 2, 2);
    display.println(_title);

    display.setFont(LCD_Font);
    int y = th + 12 + 2;

    // display.setTextSize(2);
    // int y = th + 5;

    int n = _items.size();
    int end = min(_scrollOffset + MENU_VISIBLE_LINES, n);
    for (int i = _scrollOffset; i < end; ++i)
    {

        if (i == _selected)
        {

            static const unsigned char PROGMEM pointer_bmp[] =
                {
                    0b01100000,
                    0b00111000,
                    0b00001110,
                    0b11111111,
                    0b11111111,
                    0b00001110,
                    0b00111000,
                    0b01100000,
                };
            display.drawBitmap(2, y - 8, pointer_bmp, 8, 8, SSD1306_WHITE);
        }
        display.setCursor(12, y);

        auto &item = _items[i];
        display.print(item.label);
        // Show value for edit items
        if (item.type == MENU_EDIT_INT && item.editInt)
        {
            display.print(": ");
            display.print(*(item.editInt));
            if (_editing && i == _selected)
                display.print(" [edit]");
        }
        if (item.type == MENU_EDIT_BOOL && item.editBool)
        {
            display.print(": ");
            display.print(*(item.editBool) ? "ON" : "OFF");
            if (_editing && i == _selected)
                display.print(" [edit]");
        }
        if (item.type == MENU_SUBMENU)
        {
            display.print("...");
        }

        // if (i == _selected){
        //     display.pix
        // }
        // display.print("> ");
        // else display.print("   ");
        y += 13;
    }
    display.display();
}

// ----------- Popup ---------------
// Popup::Popup(const String& msg, uint32_t timeoutMs)
//     : _msg(msg), _timeoutMs(timeoutMs), _startMs(0) ,_pagestackpop(false){}
Popup::Popup(const String &msg, uint32_t timeoutMs, bool pagestackpop)
    : _msg(msg), _timeoutMs(timeoutMs), _startMs(0), _pagestackpop(pagestackpop) {}

void Popup::onEnter()
{
    _startMs = millis();
}
void Popup::update() {}
void Popup::draw(Adafruit_SSD1306 &display)
{
    display.clearDisplay();
    // display.setTextSize(2);
    display.setFont(LCD_Font);
    display.setCursor(10, LCD_HEIGHT / 2 - 10);
    display.setTextColor(SSD1306_WHITE);
    display.println(_msg);
    display.display();
}
void Popup::onKey(LCDCustomKeypad e)
{
    // prepare for expire
    _timeoutMs = 0;
}
bool Popup::isExpired() const
{
    return millis() - _startMs > _timeoutMs;
}
bool Popup::hasPageStackPop() const
{
    return _pagestackpop;
}

// ----------- IndustrialLcdUI ---------------
void ConfirmationPopup::onKey(LCDCustomKeypad k)
{
    if (k == LCD_KEYPAD_EVENT_LEFT || k == LCD_KEYPAD_EVENT_RIGHT)
        _choice = !_choice;
    else if (k == LCD_KEYPAD_EVENT_OK)
    {
        if (_callback)
            _callback(_choice);
        // Dismiss handled by UI logic
    }
}
void ConfirmationPopup::draw(Adafruit_SSD1306 &display)
{
    display.clearDisplay();
    // display.setTextSize(2);
    display.setFont(LCD_Font);
    display.setCursor(10, 20);
    display.println(_msg);
    display.setCursor(10, 40);
    display.print(_choice ? "[Yes]  No" : " Yes  [No]");
    display.display();
}

// ----------- IndustrialLcdUI ---------------
IndustrialLcdUI::IndustrialLcdUI(Adafruit_SSD1306 &display, Keypad &keypad)
    : _display(display), _keypad(keypad), _popup(nullptr),
      _statusBarProvider(nullptr), _lastInputMs(0), _sleepTimeoutMs(60000), _sleeping(false), _font(LCD_Font) {}

bool IndustrialLcdUI::begin()
{

    _keypad.begin();
    if (!_display.begin(SSD1306_SWITCHCAPVCC))
        return false;
    _display.clearDisplay();
    _display.display();
    _lastInputMs = millis();
    return true;
}

void IndustrialLcdUI::loop()
{
    _keypad.poll();
    processKeypad();

    // UI logic
    if (_popup)
    {
        _popup->update();
        _popup->draw(_display);
        if (_popup->isExpired())
        {
            bool haspop = _popup->hasPageStackPop();
            delete _popup;
            _popup = nullptr;
            if (haspop)
            {
                popPage();
            }
        }
        return;
    }

    if (_pageStack.empty())
        return;
    auto *page = _pageStack.back();
    page->update();

    if (!_sleeping)
    {
        page->draw(_display);
        // Draw status bar if provided
        if (_statusBarProvider)
            _statusBarProvider(_display);
    }

    // Sleep logic
    if (_sleepTimeoutMs > 0 && millis() - _lastInputMs > _sleepTimeoutMs)
    {
        if (!_sleeping)
        {
            _display.ssd1306_command(SSD1306_DISPLAYOFF);
            _sleeping = true;
        }
    }
}

void IndustrialLcdUI::pushPage(UIPage *page)
{
    if (!_pageStack.empty())
        _pageStack.back()->onExit();
    _pageStack.push_back(page);
    page->onEnter();
    _display.display();
    wake();
}
void IndustrialLcdUI::popPage()
{
    if (!_pageStack.empty())
    {
        _pageStack.back()->onExit();
        _pageStack.pop_back();
        if (!_pageStack.empty())
            _pageStack.back()->onEnter();
    }
    wake();
}
UIPage *IndustrialLcdUI::currentPage()
{
    if (_pageStack.empty())
        return nullptr;
    return _pageStack.back();
}

void IndustrialLcdUI::showPopup(const String &msg, uint32_t timeoutMs, bool hasPagePop)
{
    if (_popup)
        delete _popup;
    _popup = new Popup(msg, timeoutMs, hasPagePop);
    _popup->onEnter();
    wake();
}

void IndustrialLcdUI::setStatusBarProvider(std::function<void(Adafruit_SSD1306 &)> provider)
{
    _statusBarProvider = provider;
}
void IndustrialLcdUI::setSleepTimeout(uint32_t ms)
{
    _sleepTimeoutMs = ms;
}

void IndustrialLcdUI::processKeypad()
{
    _keypad.processEvents(); // Make sure keypad is up to date
    // Only handle PRESS event for navigation, but expand as needed
    // Assumes OK = 0, LEFT = 1, RIGHT = 2
    _keypad.onAny([this](const KeypadEvent &evt)
                  {
        if (!_pageStack.empty() && !_popup && evt.type == KEYPAD_EVENT_PRESS) {
            if (evt.buttonIdx == 0) // OK
                _pageStack.back()->onKey(LCD_KEYPAD_EVENT_OK);
            else if (evt.buttonIdx == 1) // LEFT
                _pageStack.back()->onKey(LCD_KEYPAD_EVENT_LEFT);
            else if (evt.buttonIdx == 2) // RIGHT
                _pageStack.back()->onKey(LCD_KEYPAD_EVENT_RIGHT);
            _lastInputMs = millis();
            wake();
        } });
}

void IndustrialLcdUI::wake()
{
    if (_sleeping)
    {
        _display.ssd1306_command(SSD1306_DISPLAYON);
        _sleeping = false;
    }
    _lastInputMs = millis();
}
