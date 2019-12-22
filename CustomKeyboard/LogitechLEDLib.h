//
// Logitech Gaming LED SDK
//
// Copyright (C) 2011-2014 Logitech. All rights reserved.
// Author: Tiziano Pigliucci
// Email: devtechsupport@logitech.com

#pragma once

#define LOGI_LED_BITMAP_WIDTH 21
#define LOGI_LED_BITMAP_HEIGHT 6
#define LOGI_LED_BITMAP_BYTES_PER_KEY 4

#define LOGI_LED_BITMAP_SIZE (LOGI_LED_BITMAP_WIDTH*LOGI_LED_BITMAP_HEIGHT*LOGI_LED_BITMAP_BYTES_PER_KEY)

#define LOGI_LED_DURATION_INFINITE 0

#define LOGI_DEVICETYPE_MONOCHROME_ORD  0
#define LOGI_DEVICETYPE_RGB_ORD         1
#define LOGI_DEVICETYPE_PERKEY_RGB_ORD  2

#define LOGI_DEVICETYPE_MONOCHROME  (1 << LOGI_DEVICETYPE_MONOCHROME_ORD)
#define LOGI_DEVICETYPE_RGB         (1 << LOGI_DEVICETYPE_RGB_ORD)
#define LOGI_DEVICETYPE_PERKEY_RGB  (1 << LOGI_DEVICETYPE_PERKEY_RGB_ORD)

#define LOGI_DEVICETYPE_ALL (LOGI_DEVICETYPE_MONOCHROME | LOGI_DEVICETYPE_RGB | LOGI_DEVICETYPE_PERKEY_RGB)

#include <map>
#include "Color.h"

namespace LogiLed
{
    typedef enum class KeyName {
        ESC                     = 0x01,
        F1                      = 0x3b,
        F2                      = 0x3c,
        F3                      = 0x3d,
        F4                      = 0x3e,
        F5                      = 0x3f,
        F6                      = 0x40,
        F7                      = 0x41,
        F8                      = 0x42,
        F9                      = 0x43,
        F10                     = 0x44,
        F11                     = 0x57,
        F12                     = 0x58,
        PRINT_SCREEN            = 0x137,
        SCROLL_LOCK             = 0x46,
        PAUSE_BREAK             = 0x145,
        TILDE                   = 0x29,
        ONE                     = 0x02,
        TWO                     = 0x03,
        THREE                   = 0x04,
        FOUR                    = 0x05,
        FIVE                    = 0x06,
        SIX                     = 0x07,
        SEVEN                   = 0x08,
        EIGHT                   = 0x09,
        NINE                    = 0x0A,
        ZERO                    = 0x0B,
        MINUS                   = 0x0C,
        EQUALS                  = 0x0D,
        BACKSPACE               = 0x0E,
        INSERT                  = 0x152,
        HOME                    = 0x147,
        PAGE_UP                 = 0x149,
        NUM_LOCK                = 0x45,
        NUM_SLASH               = 0x135,
        NUM_ASTERISK            = 0x37,
        NUM_MINUS               = 0x4A,
        TAB                     = 0x0F,
        Q                       = 0x10,
        W                       = 0x11,
        E                       = 0x12,
        R                       = 0x13,
        T                       = 0x14,
        Y                       = 0x15,
        U                       = 0x16,
        I                       = 0x17,
        O                       = 0x18,
        P                       = 0x19,
        OPEN_BRACKET            = 0x1A,
        CLOSE_BRACKET           = 0x1B,
        BACKSLASH               = 0x2B,
        KEYBOARD_DELETE         = 0x153,
        END                     = 0x14F,
        PAGE_DOWN               = 0x151,
        NUM_SEVEN               = 0x47,
        NUM_EIGHT               = 0x48,
        NUM_NINE                = 0x49,
        NUM_PLUS                = 0x4E,
        CAPS_LOCK               = 0x3A,
        A                       = 0x1E,
        S                       = 0x1F,
        D                       = 0x20,
        F                       = 0x21,
        G                       = 0x22,
        H                       = 0x23,
        J                       = 0x24,
        K                       = 0x25,
        L                       = 0x26,
        SEMICOLON               = 0x27,
        APOSTROPHE              = 0x28,
        ENTER                   = 0x1C,
        NUM_FOUR                = 0x4B,
        NUM_FIVE                = 0x4C,
        NUM_SIX                 = 0x4D,
        LEFT_SHIFT              = 0x2A,
        Z                       = 0x2C,
        X                       = 0x2D,
        C                       = 0x2E,
        V                       = 0x2F,
        B                       = 0x30,
        N                       = 0x31,
        M                       = 0x32,
        COMMA                   = 0x33,
        PERIOD                  = 0x34,
        FORWARD_SLASH           = 0x35,
        RIGHT_SHIFT             = 0x36,
        ARROW_UP                = 0x148,
        NUM_ONE                 = 0x4F,
        NUM_TWO                 = 0x50,
        NUM_THREE               = 0x51,
        NUM_ENTER               = 0x11C,
        LEFT_CONTROL            = 0x1D,
        LEFT_WINDOWS            = 0x15B,
        LEFT_ALT                = 0x38,
        SPACE                   = 0x39,
        RIGHT_ALT               = 0x138,
        RIGHT_WINDOWS           = 0x15C,
        APPLICATION_SELECT      = 0x15D,
        RIGHT_CONTROL           = 0x11D,
        ARROW_LEFT              = 0x14B,
        ARROW_DOWN              = 0x150,
        ARROW_RIGHT             = 0x14D,
        NUM_ZERO                = 0x52,
        NUM_PERIOD              = 0x53,
        G_1                     = 0xFFF1,
        G_2                     = 0xFFF2,
        G_3                     = 0xFFF3,
        G_4                     = 0xFFF4,
        G_5                     = 0xFFF5,
        G_6                     = 0xFFF6,
        G_7                     = 0xFFF7,
        G_8                     = 0xFFF8,
        G_9                     = 0xFFF9,
        G_LOGO                  = 0xFFFF1,
        G_BADGE                 = 0xFFFF2

    }KeyName;
    static const KeyName allKeys[] = {
        KeyName::ESC,
        KeyName::F1,
        KeyName::F2,
        KeyName::F3,
        KeyName::F4,
        KeyName::F5,
        KeyName::F6,
        KeyName::F7,
        KeyName::F8,
        KeyName::F9,
        KeyName::F10,
        KeyName::F11,
        KeyName::F12,
        KeyName::PRINT_SCREEN,
        KeyName::SCROLL_LOCK,
        KeyName::PAUSE_BREAK,
        KeyName::TILDE,
        KeyName::ONE,
        KeyName::TWO,
        KeyName::THREE,
        KeyName::FOUR,
        KeyName::FIVE,
        KeyName::SIX,
        KeyName::SEVEN,
        KeyName::EIGHT,
        KeyName::NINE,
        KeyName::ZERO,
        KeyName::MINUS,
        KeyName::EQUALS,
        KeyName::BACKSPACE,
        KeyName::INSERT,
        KeyName::HOME,
        KeyName::PAGE_UP,
        KeyName::NUM_LOCK,
        KeyName::NUM_SLASH,
        KeyName::NUM_ASTERISK,
        KeyName::NUM_MINUS,
        KeyName::TAB,
        KeyName::Q,
        KeyName::W,
        KeyName::E,
        KeyName::R,
        KeyName::T,
        KeyName::Y,
        KeyName::U,
        KeyName::I,
        KeyName::O,
        KeyName::P,
        KeyName::OPEN_BRACKET,
        KeyName::CLOSE_BRACKET,
        KeyName::BACKSLASH,
        KeyName::KEYBOARD_DELETE,
        KeyName::END,
        KeyName::PAGE_DOWN,
        KeyName::NUM_SEVEN,
        KeyName::NUM_EIGHT,
        KeyName::NUM_NINE,
        KeyName::NUM_PLUS,
        KeyName::CAPS_LOCK,
        KeyName::A,
        KeyName::S,
        KeyName::D,
        KeyName::F,
        KeyName::G,
        KeyName::H,
        KeyName::J,
        KeyName::K,
        KeyName::L,
        KeyName::SEMICOLON,
        KeyName::APOSTROPHE,
        KeyName::ENTER,
        KeyName::NUM_FOUR,
        KeyName::NUM_FIVE,
        KeyName::NUM_SIX,
        KeyName::LEFT_SHIFT,
        KeyName::Z,
        KeyName::X,
        KeyName::C,
        KeyName::V,
        KeyName::B,
        KeyName::N,
        KeyName::M,
        KeyName::COMMA,
        KeyName::PERIOD,
        KeyName::FORWARD_SLASH,
        KeyName::RIGHT_SHIFT,
        KeyName::ARROW_UP,
        KeyName::NUM_ONE,
        KeyName::NUM_TWO,
        KeyName::NUM_THREE,
        KeyName::NUM_ENTER,
        KeyName::LEFT_CONTROL,
        KeyName::LEFT_WINDOWS,
        KeyName::LEFT_ALT,
        KeyName::SPACE,
        KeyName::RIGHT_ALT,
        KeyName::RIGHT_WINDOWS,
        KeyName::APPLICATION_SELECT,
        KeyName::RIGHT_CONTROL,
        KeyName::ARROW_LEFT,
        KeyName::ARROW_DOWN,
        KeyName::ARROW_RIGHT,
        KeyName::NUM_ZERO,
        KeyName::NUM_PERIOD,
        KeyName::G_1,
        KeyName::G_2,
        KeyName::G_3,
        KeyName::G_4,
        KeyName::G_5,
        KeyName::G_6,
        KeyName::G_7,
        KeyName::G_8,
        KeyName::G_9,
        KeyName::G_LOGO,
        KeyName::G_BADGE
    };
    const std::map<KeyName, int> bitmapIndex = {    //note that the g keys, badge, and logo are not in bitmap
        {KeyName::ESC, 0},
        {KeyName::F1, 4},
        {KeyName::F2, 8},
        {KeyName::F3, 12},
        {KeyName::F4, 16},
        {KeyName::F5, 20},
        {KeyName::F6, 24},
        {KeyName::F7, 28},
        {KeyName::F8, 32},
        {KeyName::F9, 36},
        {KeyName::F10, 40},
        {KeyName::F11, 44},
        {KeyName::F12, 48},
        {KeyName::PRINT_SCREEN, 52},
        {KeyName::SCROLL_LOCK, 56},
        {KeyName::PAUSE_BREAK, 60},
        {KeyName::TILDE, 84},
        {KeyName::ONE, 88},
        {KeyName::TWO, 92},
        {KeyName::THREE, 96},
        {KeyName::FOUR, 100},
        {KeyName::FIVE, 104},
        {KeyName::SIX, 108},
        {KeyName::SEVEN, 112},
        {KeyName::EIGHT, 116},
        {KeyName::NINE, 120},
        {KeyName::ZERO, 124},
        {KeyName::MINUS, 128},
        {KeyName::EQUALS, 132},
        {KeyName::BACKSPACE, 136},
        {KeyName::INSERT, 140},
        {KeyName::HOME, 144},
        {KeyName::PAGE_UP, 148},
        {KeyName::NUM_LOCK, 152},
        {KeyName::NUM_SLASH, 156},
        {KeyName::NUM_ASTERISK, 160},
        {KeyName::NUM_MINUS, 164},
        {KeyName::TAB, 168},
        {KeyName::Q, 172},
        {KeyName::W, 176},
        {KeyName::E, 180},
        {KeyName::R, 184},
        {KeyName::T, 188},
        {KeyName::Y, 192},
        {KeyName::U, 196},
        {KeyName::I, 200},
        {KeyName::O, 204},
        {KeyName::P, 208},
        {KeyName::OPEN_BRACKET, 212},
        {KeyName::CLOSE_BRACKET, 216},
        {KeyName::BACKSLASH, 220},
        {KeyName::KEYBOARD_DELETE, 224},
        {KeyName::END, 228},
        {KeyName::PAGE_DOWN, 232},
        {KeyName::NUM_SEVEN, 236},
        {KeyName::NUM_EIGHT, 240},
        {KeyName::NUM_NINE, 244},
        {KeyName::NUM_PLUS, 248},
        {KeyName::CAPS_LOCK, 252},
        {KeyName::A, 256},
        {KeyName::S, 260},
        {KeyName::D, 264},
        {KeyName::F, 268},
        {KeyName::G, 272},
        {KeyName::H, 276},
        {KeyName::J, 280},
        {KeyName::K, 284},
        {KeyName::L, 288},
        {KeyName::SEMICOLON, 292},
        {KeyName::APOSTROPHE, 296},
        {KeyName::ENTER, 304},
        {KeyName::NUM_FOUR, 320},
        {KeyName::NUM_FIVE, 324},
        {KeyName::NUM_SIX, 328},
        {KeyName::LEFT_SHIFT, 336},
        {KeyName::Z, 344},
        {KeyName::X, 348},
        {KeyName::C, 352},
        {KeyName::V, 356},
        {KeyName::B, 360},
        {KeyName::N, 364},
        {KeyName::M, 368},
        {KeyName::COMMA, 372},
        {KeyName::PERIOD, 376},
        {KeyName::FORWARD_SLASH, 380},
        {KeyName::RIGHT_SHIFT, 388},
        {KeyName::ARROW_UP, 396},
        {KeyName::NUM_ONE, 404},
        {KeyName::NUM_TWO, 408},
        {KeyName::NUM_THREE, 412},
        {KeyName::NUM_ENTER, 416},
        {KeyName::LEFT_CONTROL, 420},
        {KeyName::LEFT_WINDOWS, 424},
        {KeyName::LEFT_ALT, 428},
        {KeyName::SPACE, 440},
        {KeyName::RIGHT_ALT, 464},
        {KeyName::RIGHT_WINDOWS, 468},
        {KeyName::APPLICATION_SELECT, 472},
        {KeyName::RIGHT_CONTROL, 476},
        {KeyName::ARROW_LEFT, 480},
        {KeyName::ARROW_DOWN, 484},
        {KeyName::ARROW_RIGHT, 488},
        {KeyName::NUM_ZERO, 492},
        {KeyName::NUM_PERIOD, 496}
    };
    const std::map<KeyName, int> extraKeyIndex = {
        {KeyName::G_1, 0}
    };

    typedef enum
    {
        Keyboard                = 0x0,
        Mouse                   = 0x3,
        Mousemat                = 0x4,
        Headset                 = 0x8,
        Speaker                 = 0xe
    }DeviceType;
}


struct ColorMap {
    std::vector<unsigned char>* bitmap = new std::vector<unsigned char>(LOGI_LED_BITMAP_SIZE);
    Color G1;
    Color G2;
    Color G3;
    Color G4;
    Color G5;
    Color G6;
    Color G7;
    Color G8;
    Color G9;
    Color Badge;
    Color Logo;
    Color Mouse0;
    Color Mouse1;

    ColorMap() : G1(0), G2(0), G3(0), G4(0), G5(0), G6(0), G7(0), G8(0), G9(0), Badge(0), Logo(0), Mouse0(0), Mouse1(0) {}
};

bool LogiLedInit();
bool LogiLedInitWithName(const char name[]);

bool LogiLedGetSdkVersion(int *majorNum, int *minorNum, int *buildNum);
bool LogiLedGetConfigOptionNumber(const wchar_t *configPath, double *defaultValue);
bool LogiLedGetConfigOptionBool(const wchar_t *configPath, bool *defaultValue);
bool LogiLedGetConfigOptionColor(const wchar_t *configPath, int *defaultRed, int *defaultGreen, int *defaultBlue);
bool LogiLedGetConfigOptionRect(const wchar_t *configPath, int *defaultX, int *defaultY, int *defaultWidth, int *defaultHeight);
bool LogiLedGetConfigOptionString(const wchar_t *configPath, wchar_t *defaultValue, int bufferSize);
bool LogiLedGetConfigOptionKeyInput(const wchar_t *configPath, wchar_t *defaultValue, int bufferSize);
bool LogiLedGetConfigOptionSelect(const wchar_t *configPath, wchar_t *defaultValue, int *valueSize, const wchar_t *values, int bufferSize);
bool LogiLedGetConfigOptionRange(const wchar_t *configPath, int *defaultValue, int min, int max);
bool LogiLedSetConfigOptionLabel(const wchar_t *configPath, wchar_t *label);

//Generic functions => Apply to any device type.
bool LogiLedSetTargetDevice(int targetDevice);
bool LogiLedSaveCurrentLighting();
bool LogiLedSetLighting(int redPercentage, int greenPercentage, int bluePercentage);
bool LogiLedRestoreLighting();
bool LogiLedFlashLighting(int redPercentage, int greenPercentage, int bluePercentage, int milliSecondsDuration, int milliSecondsInterval);
bool LogiLedPulseLighting(int redPercentage, int greenPercentage, int bluePercentage, int milliSecondsDuration, int milliSecondsInterval);
bool LogiLedStopEffects();

//Per-key functions => only apply to LOGI_DEVICETYPE_PERKEY_RGB devices.
bool LogiLedSetLightingFromBitmap(unsigned char bitmap[]);
bool LogiLedSetLightingForKeyWithScanCode(int keyCode, int redPercentage, int greenPercentage, int bluePercentage);
bool LogiLedSetLightingForKeyWithHidCode(int keyCode, int redPercentage, int greenPercentage, int bluePercentage);
bool LogiLedSetLightingForKeyWithQuartzCode(int keyCode, int redPercentage, int greenPercentage, int bluePercentage);
bool LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName keyName, int redPercentage, int greenPercentage, int bluePercentage);
bool LogiLedSaveLightingForKey(LogiLed::KeyName keyName);
bool LogiLedRestoreLightingForKey(LogiLed::KeyName keyName);
bool LogiLedExcludeKeysFromBitmap(LogiLed::KeyName *keyList, int listCount);

//Per-key effects => only apply to LOGI_DEVICETYPE_PERKEY_RGB devices.
bool LogiLedFlashSingleKey(LogiLed::KeyName keyName, int redPercentage, int greenPercentage, int bluePercentage, int msDuration, int msInterval);
bool LogiLedPulseSingleKey(LogiLed::KeyName keyName, int startRedPercentage, int startGreenPercentage, int startBluePercentage, int finishRedPercentage, int finishGreenPercentage, int finishBluePercentage, int msDuration, bool isInfinite);
bool LogiLedStopEffectsOnKey(LogiLed::KeyName keyName);

//Zonal functions => only apply to devices with zones.
bool LogiLedSetLightingForTargetZone(LogiLed::DeviceType deviceType, int zone, int redPercentage, int greenPercentage, int bluePercentage);

//overloaded functions to apply a Color object
bool LogiLedSetLighting(Color& c);
bool LogiLedFlashLighting(Color& c, int msDur, int msInt);
bool LogiLedPulseLighting(Color& c, int msDur, int msInt);

bool LogiLedSetLightingForKeyWithScanCode(int keyCode, Color& c);
bool LogiLedSetLightingForKeyWithHidCode(int keyCode, Color& c);
bool LogiLedSetLightingForKeyWithQuartzCode(int keyCode, Color& c);
bool LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName keyName, Color& c);

bool LogiLedFlashSingleKey(LogiLed::KeyName keyName, Color& c, int msDur, int msInt);
bool LogiLedPulseSingleKey(LogiLed::KeyName keyName, Color& c1, Color& c2, int msDur, int msInt);

void LogiLedShutdown();
