#pragma once

#include <dinput.h>
#include <Xinput.h>
#include <wrl/client.h>

#include <cstdint>

#define MOUSE_BUTTON_LEFT  0
#define MOUSE_BUTTON_RIGHT 1
#define MOUSE_BUTTON_MID   2

#define MOUSE_CLICKED_VALUE 0x80

class Input {
public:

    static Input* GetInstance();

    void Initialize(HWND hWnd);
    void Update();

    bool IsKeyPressed(unsigned char keycode) const { return keys_[keycode] != 0; }
    bool IsKeyTrigger(unsigned char keycode) const { return keys_[keycode] != 0 && preKeys_[keycode] == 0; }
    bool IsKeyRelease(unsigned char keycode) const { return keys_[keycode] == 0 && preKeys_[keycode] != 0; }

    bool IsMousePressed(int button) const { return mouseState_.state.rgbButtons[button] & MOUSE_CLICKED_VALUE; }
    bool IsMouseTrigger(int button) const { return (mouseState_.state.rgbButtons[button] & MOUSE_CLICKED_VALUE) && !(preMouseState_.state.rgbButtons[button] & MOUSE_CLICKED_VALUE); }
    bool IsMouseRelease(int button) const { return !(mouseState_.state.rgbButtons[button] & MOUSE_CLICKED_VALUE) && (preMouseState_.state.rgbButtons[button] & MOUSE_CLICKED_VALUE); }
    POINT GetMousePosition() const { return mouseState_.screenPos; }

    LONG GetMouseMoveX() const { return mouseState_.state.lX; }
    LONG GetMouseMoveY() const { return mouseState_.state.lY; }
    LONG GetMouseWheel() const { return mouseState_.state.lZ; }

    const XINPUT_STATE& GetXInputState() const { return xInputState_; }
    const XINPUT_STATE& GetPreXInputState() const { return preXInputState_; }

private:
    static const uint32_t kNumKeys = 256;
   
    Input() = default;
    Input(const Input&) = delete;
    Input& operator=(const Input&) = delete;

    struct MouseState {
        DIMOUSESTATE state;
        POINT screenPos;
    };

    HWND hWnd_ = nullptr;
    Microsoft::WRL::ComPtr<IDirectInput8> directInput_;
    Microsoft::WRL::ComPtr<IDirectInputDevice8> keybord_;
    Microsoft::WRL::ComPtr<IDirectInputDevice8> mouse_;

    uint8_t keys_[kNumKeys]{};
    uint8_t preKeys_[kNumKeys]{};

    MouseState mouseState_{};
    MouseState preMouseState_{};

    XINPUT_STATE xInputState_{};
    XINPUT_STATE preXInputState_{};
};

#ifdef MOUSE_CLICKED_VALUE
#undef MOUSE_CLICKED_VALUE
#endif // MOUSE_CLICKED_VALUE
