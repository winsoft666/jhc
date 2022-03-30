/*******************************************************************************
*    C++ Common Library
*    ---------------------------------------------------------------------------
*    Copyright (C) 2022 JiangXueqiao <winsoft666@outlook.com>.
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/
#ifndef JHC_WIN_VIRTUAL_KEY_HPP__
#define JHC_WIN_VIRTUAL_KEY_HPP__
#include <string>
#include <unordered_map>

namespace jhc {
class WinVirtualKey {
   public:
    static std::string ToString(unsigned int vk) {
        for (const auto& k : vkMaps_) {
            if (k.second == vk) {
                return k.first;
            }
        }
        return "";
    }

    static unsigned int ToInteger(const std::string& s) {
        const auto it = vkMaps_.find(s);
        if (it != vkMaps_.end())
            return it->second;
        return 0;
    }

   private:
    static std::unordered_map<std::string, unsigned int> vkMaps_;
};

std::unordered_map<std::string, unsigned int> WinVirtualKey::vkMaps_ = {
    {"VK_LBUTTON", 0x01},             /*Left MouseButton*/
    {"VK_RBUTTON", 0x02},             /*Right MouseButton*/
    {"VK_MBUTTON", 0x04},             /*Middle MouseButton*/
    {"VK_XBUTTON1", 0x05},            /*X1 MouseButton*/
    {"VK_XBUTTON2", 0x06},            /*X2 MouseButton*/
    {"VK_CANCEL", 0x03},              /*Control-break processing*/
    {"VK_BACK", 0x08},                /*Backspace Key*/
    {"VK_TAB", 0x09},                 /*Tab key*/
    {"VK_CLEAR", 0x0C},               /*CLEAR key*/
    {"VK_RETURN", 0x0D},              /*ENTER key*/
    {"VK_SHIFT", 0x10},               /*SHIFT key*/
    {"VK_CONTROL", 0x11},             /*CTRL key*/
    {"VK_MENU", 0x12},                /*ALT key*/
    {"VK_PAUSE", 0x13},               /*PAUSE key*/
    {"VK_CAPITAL", 0x14},             /*CAPS LOCK key*/
    {"VK_KANA", 0x15},                /*IME kana mode*/
    {"VK_HANGUEL", 0x15},             /*IME Hanguel mode (maintained for campatibility: use K_HANGUL)*/
    {"VK_HANGUL", 0x15},              /*IME Hangul mode*/
    {"VK_IME_ON", 0x16},              /*IME On*/
    {"VK_JUNJA", 0x17},               /*IME Junja mode*/
    {"VK_FINAL", 0x18},               /*IME Final mode*/
    {"VK_HANJA", 0x19},               /*IME Hanja mode*/
    {"VK_KANJI", 0x19},               /*IME Kanjo mode*/
    {"VK_IME_OFF", 0x1A},             /*IME Off*/
    {"VK_ESCAPE", 0x1B},              /*ESC key*/
    {"VK_CONVERT", 0x1C},             /*IME convert*/
    {"VK_NONCONVERT", 0x1D},          /*IME nonconvert*/
    {"VK_ACCEPT", 0x1E},              /*IME accept*/
    {"VK_MODECHANGE", 0x1F},          /*IME mode change request*/
    {"VK_SPACE", 0x20},               /*SPACE key*/
    {"VK_PRIOR", 0x21},               /*PAGE UP key*/
    {"VK_NEXT", 0x22},                /*PAGE DOWN key*/
    {"VK_END", 0x23},                 /*END key*/
    {"VK_HOME", 0x24},                /*HOME key*/
    {"VK_LEFT", 0x25},                /*LEFT ARROW key*/
    {"VK_UP", 0x26},                  /*UP ARROW key*/
    {"VK_RIGHT", 0x27},               /*RIGHT ARROW key*/
    {"VK_DOWN", 0x28},                /*DOWN ARROW key*/
    {"VK_SELECT", 0x29},              /*SELECT key*/
    {"VK_PRINT", 0x2A},               /*PRINT key*/
    {"VK_EXECUTE", 0x2B},             /*EXECUTE key*/
    {"VK_SNAPSHOT", 0x2C},            /*PRINT SCREEN key*/
    {"VK_INSERT", 0x2D},              /*INS key*/
    {"VK_DELETE", 0x2E},              /*DEL key*/
    {"VK_HELP", 0x2F},                /*HELP key*/
    {"VK_0", 0x30},                   /*0 key*/
    {"VK_1", 0x31},                   /*1 key*/
    {"VK_2", 0x32},                   /*2 key*/
    {"VK_3", 0x33},                   /*3 key*/
    {"VK_4", 0x34},                   /*4 key*/
    {"VK_5", 0x35},                   /*5 key*/
    {"VK_6", 0x36},                   /*6 key*/
    {"VK_7", 0x37},                   /*7 key*/
    {"VK_8", 0x38},                   /*8 key*/
    {"VK_9", 0x39},                   /*9 key*/
    {"VK_A", 0x41},                   /*A key*/
    {"VK_B", 0x42},                   /*B key*/
    {"VK_C", 0x43},                   /*C key*/
    {"VK_D", 0x44},                   /*D key*/
    {"VK_E", 0x45},                   /*E key*/
    {"VK_F", 0x46},                   /*F key*/
    {"VK_G", 0x47},                   /*G key*/
    {"VK_H", 0x48},                   /*H key*/
    {"VK_I", 0x49},                   /*I key*/
    {"VK_J", 0x4A},                   /*J key*/
    {"VK_K", 0x4B},                   /*K key*/
    {"VK_L", 0x4C},                   /*L key*/
    {"VK_M", 0x4D},                   /*M key*/
    {"VK_N", 0x4E},                   /*N key*/
    {"VK_O", 0x4F},                   /*O key*/
    {"VK_P", 0x50},                   /*P key*/
    {"VK_Q", 0x51},                   /*Q key*/
    {"VK_R", 0x52},                   /*R key*/
    {"VK_S", 0x53},                   /*S key*/
    {"VK_T", 0x54},                   /*T key*/
    {"VK_U", 0x55},                   /*U key*/
    {"VK_V", 0x56},                   /*V key*/
    {"VK_W", 0x57},                   /*W key*/
    {"VK_X", 0x58},                   /*X key*/
    {"VK_Y", 0x59},                   /*Y key*/
    {"VK_Z", 0x5A},                   /*Z key*/
    {"VK_LWIN", 0x5B},                /*Left Windows key (Natural keyboard)*/
    {"VK_RWIN", 0x5C},                /*Right Windows key (Natural keyboard)*/
    {"VK_APPS", 0x5D},                /*Application key (Natural keyboard)*/
    {"VK_SLEEP", 0x5F},               /*Computer Sleep key*/
    {"VK_NUM0", 0x60},                /*Numeric keypad 0 key*/
    {"VK_NUM1", 0x61},                /*Numeric keypad 1 key*/
    {"VK_NUM2", 0x62},                /*Numeric keypad 2 key*/
    {"VK_NUM3", 0x63},                /*Numeric keypad 3 key*/
    {"VK_NUM4", 0x64},                /*Numeric keypad 4 key*/
    {"VK_NUM5", 0x65},                /*Numeric keypad 5 key*/
    {"VK_NUM6", 0x66},                /*Numeric keypad 6 key*/
    {"VK_NUM7", 0x67},                /*Numeric keypad 7 key*/
    {"VK_NUM8", 0x68},                /*Numeric keypad 8 key*/
    {"VK_NUM9", 0x69},                /*Numeric keypad 9 key*/
    {"VK_MULTIPLY", 0x6A},            /*Multiply key*/
    {"VK_ADD", 0x6B},                 /*Add key*/
    {"VK_SEPERATOR", 0x6C},           /*Seperator key*/
    {"VK_SUBTRACT", 0x6D},            /*Subtract key*/
    {"VK_DECIMAL", 0x6E},             /*Decimal key*/
    {"VK_DIVIDE", 0x6F},              /*Divide key*/
    {"VK_F1", 0x70},                  /*F1 Key*/
    {"VK_F2", 0x71},                  /*F2 Key*/
    {"VK_F3", 0x72},                  /*F3 Key*/
    {"VK_F4", 0x73},                  /*F4 Key*/
    {"VK_F5", 0x74},                  /*F5 Key*/
    {"VK_F6", 0x75},                  /*F6 Key*/
    {"VK_F7", 0x76},                  /*F7 Key*/
    {"VK_F8", 0x77},                  /*F8 Key*/
    {"VK_F9", 0x78},                  /*F9 Key*/
    {"VK_F10", 0x79},                 /*F10 Key*/
    {"VK_F11", 0x7A},                 /*F11 Key*/
    {"VK_F12", 0x7B},                 /*F12 Key*/
    {"VK_F13", 0x7C},                 /*F13 Key*/
    {"VK_F14", 0x7D},                 /*F14 Key*/
    {"VK_F15", 0x7E},                 /*F15 Key*/
    {"VK_F16", 0x7F},                 /*F16 Key*/
    {"VK_F17", 0x80},                 /*F17 Key*/
    {"VK_F18", 0x81},                 /*F18 Key*/
    {"VK_F19", 0x82},                 /*F19 Key*/
    {"VK_F20", 0x83},                 /*F20 Key*/
    {"VK_F21", 0x84},                 /*F21 Key*/
    {"VK_F22", 0x85},                 /*F22 Key*/
    {"VK_F23", 0x86},                 /*F23 Key*/
    {"VK_F24", 0x87},                 /*F24 Key*/
    {"VK_NUMLOCK", 0x90},             /*NUM LOCK key*/
    {"VK_SCROLL", 0x91},              /*SCROLL LOCK key*/
    {"VK_LSHIFT", 0xA0},              /*Left SHIFT key*/
    {"VK_RSHIFT", 0xA1},              /*Left SHIFT key*/
    {"VK_LCONTROL", 0xA2},            /*Left CONTROL key*/
    {"VK_RCONTROL", 0xA3},            /*Left CONTROL key*/
    {"VK_LMENU", 0xA4},               /*Left MENU key*/
    {"VK_RMENU", 0xA5},               /*Right MENU key*/
    {"VK_BROWSER_BACK", 0xA6},        /*Browser Back key*/
    {"VK_BROWSER_FORWARD", 0xA7},     /*Browser Forward key*/
    {"VK_BROWSER_REFRESH", 0xA8},     /*Browser Refresh key*/
    {"VK_BROWSER_STOP", 0xA9},        /*Browser Stop key*/
    {"VK_BROWSER_SEARCH", 0xAA},      /*Browser Search key*/
    {"VK_BROWSER_FAVORITES", 0xAB},   /*Browser Favorites key*/
    {"VK_BROWSER_HOME", 0xAC},        /*Browser Home key*/
    {"VK_VOLUME_MUTE", 0xAD},         /*Volume Mute key*/
    {"VK_VOLUME_DOWN", 0xAE},         /*Volume Down key*/
    {"VK_VOLUME_UP", 0xAF},           /*Volume Up key*/
    {"VK_MEDIA_NEXT_TRACK", 0xB0},    /*Next Track key*/
    {"VK_MEDIA_LAST_TRACK", 0xB1},    /*Last Track key*/
    {"VK_MEDIA_STOP", 0xB2},          /*Stop Media key*/
    {"VK_MEDIA_PLAY_PAUSE", 0xB3},    /*Play/Pause Media key*/
    {"VK_LAUNCH_MAIL", 0xB4},         /*Start Mail key*/
    {"VK_LAUNCH_MEDIA_SELECT", 0xB5}, /*Select Media key*/
    {"VK_LAUNCH_APP1", 0xB6},         /*Start Application 1 key*/
    {"VK_LAUNCH_APP2", 0xB7},         /*Start Application 2 key*/
    {"VK_OEM_1", 0xBA},               /*Used for miscellaneous characters it can vary by keyboard. For the US standard keyboard, use ':' key*/
    {"VK_OEM_PLUS", 0xBB},            /*For andy country/region '+' key*/
    {"VK_OEM_COMMA", 0xBC},           /*For andy country/region ',' key*/
    {"VK_OEM_MINUS", 0xBD},           /*For andy country/region '-' key*/
    {"VK_OEM_PERIOD", 0xBE},          /*For andy country/region '.' key*/
    {"VK_OEM_2", 0xBF},               /*Used for miscellaneous characters it can vary by keyboard. For the US standard keyboard, the '/?' key*/
    {"VK_OEM_3", 0xC0},               /*Used for miscellaneous characters it can vary by keyboard. For the US standard keyboard, the '`~' key*/
    {"VK_OEM_4", 0xDB},               /*Used for miscellaneous characters it can vary by keyboard. For the US standard keyboard, the '[{' key*/
    {"VK_OEM_5", 0xDC},               /*Used for miscellaneous characters it can vary by keyboard. For the US standard keyboard, the '\|' key*/
    {"VK_OEM_6", 0xDD},               /*Used for miscellaneous characters it can vary by keyboard. For the US standard keyboard, the ']}' key*/
    {"VK_OEM_7", 0xDE},               /*Used for miscellaneous characters it can vary by keyboard. For the US standard keyboard, the 'single-quote/double-quote' key*/
    {"VK_OEM_8", 0xDF},               /*Used for miscellaneous characters it can vary by keyboard.*/
    {"VK_OEM_102", 0xE2},             /*Either the angle bracket key or the backslash key on the RT 102-key keyboard*/
    {"VK_PROCESSKEY", 0xE5},          /*IME PROCESS key*/
    {"VK_PACKET", 0xE7},              /*Used to pass Unicode characters as if they were keystrokes. The VK_PACKET key is the low word of a 32-bit Virtual Key value used for non-keyboard input methods.*/
    {"VK_ATTN", 0xF6},                /*Attn key*/
    {"VK_CRSEL", 0xF7},               /*CrSel key*/
    {"VK_EXSEL", 0xF8},               /*ExSel key*/
    {"VK_EREOF", 0xF9},               /*Erase EOF key*/
    {"VK_PLAY", 0xFA},                /*Play key*/
    {"VK_ZOOM", 0xFB},                /*Zoom key*/
    {"VK_NONAME", 0xFC},              /*Reserved*/
    {"VK_PA1", 0xFD},                 /*PA1 key*/
    {"VK_OEM_CLEAR", 0xFE}            /*Clear key*/
};
}  // namespace jhc

#endif  // !JHC_WIN_VIRTUAL_KEY_HPP__
