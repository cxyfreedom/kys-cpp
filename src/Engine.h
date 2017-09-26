﻿#pragma once

extern "C"
{
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
}

#include <algorithm>
#include <functional>
#include <vector>
#include <string>


//这里是底层部分，将SDL的函数均封装了一次
//如需更换底层，则要重新实现下面的全部功能，并重新定义全部常数和类型
#define BP_AUDIO_DEVICE_FORMAT AUDIO_S16
#define BP_AUDIO_MIX_MAXVOLUME SDL_MIX_MAXVOLUME

typedef std::function<void(uint8_t*, int)> AudioCallback;
typedef SDL_Renderer BP_Renderer;
typedef SDL_Window BP_Window;
typedef SDL_Texture BP_Texture;
typedef SDL_Rect BP_Rect;
typedef SDL_Color BP_Color;

typedef enum { BP_ALIGN_LEFT, BP_ALIGN_MIDDLE, BP_ALIGN_RIGHT } BP_Align;

#define BP_WINDOWPOS_CENTERED SDL_WINDOWPOS_CENTERED

#define RMASK (0xff0000)
#define GMASK (0xff00)
#define BMASK (0xff)
#define AMASK (0xff000000)

//声音类型在其他文件中未使用
typedef SDL_AudioSpec BP_AudioSpec;
//这里直接使用SDL的事件结构，如果更换底层需重新实现一套相同的
typedef SDL_Event BP_Event;

class Engine
{
private:
    Engine();
    virtual ~Engine();
private:
    static Engine engine_;
    Engine* this_;
public:
    static Engine* getInstance() { return &engine_; };
    //图形相关
private:
    BP_Window* window_ = nullptr;
    BP_Renderer* renderer_ = nullptr;
    BP_Texture* tex_ = nullptr, *tex2_ = nullptr, *logo_ = nullptr;
    BP_AudioSpec want_, spec_;
    BP_Rect rect_;
    BP_Texture* testTexture(BP_Texture* tex) { return tex ? tex : this->tex_; };
    bool full_screen_ = false;
    bool keep_ratio_ = true;

    int start_w_ = 768, start_h_ = 480;
    int win_w_, win_h_, min_x_, min_y_, max_x_, max_y_;
    double rotation_ = 0;
    int ratio_x_ = 1, ratio_y_ = 1;
public:
    int init(void* handle = 0);

    void getWindowSize(int& w, int& h) { SDL_GetWindowSize(window_, &w, &h); }
    void getWindowMaxSize(int& w, int& h) { SDL_GetWindowMaximumSize(window_, &w, &h); }
    int getWindowsWidth();
    int getWindowsHeight();
    int getMaxWindowWidth() { return max_x_ - min_x_; }
    int getMaxWindowHeight() { return max_y_ - min_y_; }
    void setWindowSize(int w, int h);
    void setStartWindowSize(int w, int h) { start_w_ = w; start_h_ = h; }
    void setWindowPosition(int x, int y);
    void setWindowTitle(const std::string& str) { SDL_SetWindowTitle(window_, str.c_str()); }
    BP_Renderer* getRenderer() { return renderer_; }

    void createMainTexture(int w, int h);

    void setPresentPosition();  //设置贴图的位置

    void getPresentSize(int& w, int& h) { w = rect_.w; h = rect_.h; }
    int getPresentWidth() { return rect_.w; }
    int getPresentHeight() { return rect_.h; }

    void destroyMainTexture() { destroyTexture(tex_); }

    void destroyTexture(BP_Texture* t) { SDL_DestroyTexture(t); }

    BP_Texture* createYUVTexture(int w, int h);;
    void updateYUVTexture(BP_Texture* t, uint8_t* data0, int size0, uint8_t* data1, int size1, uint8_t* data2, int size2);

    BP_Texture* createRGBATexture(int w, int h);
    BP_Texture* createRGBARenderedTexture(int w, int h);
    void updateRGBATexture(BP_Texture* t, uint8_t* buffer, int pitch);

    void renderCopy(BP_Texture* t = nullptr);
    void showLogo() { SDL_RenderCopy(renderer_, logo_, nullptr, nullptr); }
    void renderPresent() { SDL_RenderPresent(renderer_); /*renderClear();*/ }
    void renderClear() { SDL_RenderClear(renderer_); }
    void setTextureAlphaMod(BP_Texture* t, uint8_t alpha) { SDL_SetTextureAlphaMod(t, alpha); }
    void queryTexture(BP_Texture* t, int* w, int* h) { SDL_QueryTexture(t, nullptr, nullptr, w, h); }

    void createWindow() {}
    void createRenderer() {}
    void renderCopy(BP_Texture* t, int x, int y, int w = 0, int h = 0, int inPresent = 0);
    void destroy();
    bool isFullScreen();
    void toggleFullscreen();
    BP_Texture* loadImage(const std::string& filename);
    bool setKeepRatio(bool b);
    BP_Texture* transBitmapToTexture(const uint8_t* src, uint32_t color, int w, int h, int stride);
    double setRotation(double r) { return rotation_ = r; }
    void resetWindowsPosition();
    void setRatio(int x, int y) { ratio_x_ = x; ratio_y_ = y; }
    void setColor(BP_Texture* tex, BP_Color c, uint8_t alpha);
    void fillColor(BP_Color color, int x, int y, int w, int h);

    //声音相关
private:
    SDL_AudioDeviceID device_;
    AudioCallback callback_ = nullptr;
public:
    void pauseAudio(int pause) { SDL_PauseAudioDevice(device_, pause); }
    void closeAudio() { SDL_CloseAudioDevice(device_); };
    int getMaxVolume() { return BP_AUDIO_MIX_MAXVOLUME; };
    void mixAudio(Uint8* dst, const Uint8* src, Uint32 len, int volume);

    int openAudio(int& freq, int& channels, int& size, int minsize, AudioCallback f);
    static void mixAudioCallback(void* userdata, Uint8* stream, int len);
    void setAudioCallback(AudioCallback cb = nullptr) { callback_ = cb; };
    //事件相关
private:
    SDL_Event e_;
    int time_;
public:
    void delay(const int t) { SDL_Delay(t); }
    uint32_t getTicks() { return SDL_GetTicks(); }
    uint32_t tic() { return time_ = SDL_GetTicks(); }
    void toc() { if (SDL_GetTicks() != time_) { printf("%d\n", SDL_GetTicks() - time_); } }
    void getMouseState(int& x, int& y) { SDL_GetMouseState(&x, &y); };
    int pollEvent(BP_Event& e) { return SDL_PollEvent(&e); }
    int pushEvent(BP_Event& e) { return SDL_PushEvent(&e); }
    void flushEvent() { SDL_FlushEvent(0); }
    void free(void* mem) { SDL_free(mem); }
    //UI相关
private:
    BP_Texture* square_;
public:
    BP_Texture* createSquareTexture(int size);
    BP_Texture* createTextTexture(const std::string& fontname, const std::string& text, int size, BP_Color c);
    void drawText(const std::string& fontname, std::string& text, int size, int x, int y, uint8_t alpha, int align, BP_Color c);
    void drawSubtitle(const std::string& fontname, const std::string& text, int size, int x, int y, uint8_t alpha, int align);
    //void split(std::string& s, std::string& delim, std::vector< std::string >* ret);
    std::vector<std::string> splitString(const std::string& s, const std::string& delim);
    int showMessage(const std::string& content);
public:
    //标题;
    std::string title_;
};

//这里直接照搬SDL
//更换底层需自己定义一套
//好像是瞎折腾
typedef enum
{
    BP_FIRSTEVENT = SDL_FIRSTEVENT,
    //按关闭按钮
    BP_QUIT = SDL_QUIT,
    //window
    BP_WINDOWEVENT = SDL_WINDOWEVENT,
    BP_SYSWMEVENT = SDL_SYSWMEVENT,
    //键盘
    BP_KEYDOWN = SDL_KEYDOWN,
    BP_KEYUP = SDL_KEYUP,
    BP_TEXTEDITING = SDL_TEXTEDITING,
    BP_TEXTINPUT = SDL_TEXTINPUT,
    //鼠标
    BP_MOUSEMOTION = SDL_MOUSEMOTION,
    BP_MOUSEBUTTONDOWN = SDL_MOUSEBUTTONDOWN,
    BP_MOUSEBUTTONUP = SDL_MOUSEBUTTONUP,
    BP_MOUSEWHEEL = SDL_MOUSEWHEEL,
    //剪贴板
    BP_CLIPBOARDUPDATE = SDL_CLIPBOARDUPDATE,
    //拖放文件
    BP_DROPFILE = SDL_DROPFILE,
    //渲染改变
    BP_RENDER_TARGETS_RESET = SDL_RENDER_TARGETS_RESET,

    BP_LASTEVENT = SDL_LASTEVENT
} BP_EventType;

typedef enum
{
    BP_WINDOWEVENT_NONE = SDL_WINDOWEVENT_NONE,           /**< Never used */
    BP_WINDOWEVENT_SHOWN = SDL_WINDOWEVENT_SHOWN,
    BP_WINDOWEVENT_HIDDEN = SDL_WINDOWEVENT_HIDDEN,
    BP_WINDOWEVENT_EXPOSED = SDL_WINDOWEVENT_EXPOSED,

    BP_WINDOWEVENT_MOVED = SDL_WINDOWEVENT_MOVED,

    BP_WINDOWEVENT_RESIZED = SDL_WINDOWEVENT_RESIZED,
    BP_WINDOWEVENT_SIZE_CHANGED = SDL_WINDOWEVENT_SIZE_CHANGED,
    BP_WINDOWEVENT_MINIMIZED = SDL_WINDOWEVENT_MINIMIZED,
    BP_WINDOWEVENT_MAXIMIZED = SDL_WINDOWEVENT_MAXIMIZED,
    BP_WINDOWEVENT_RESTORED = SDL_WINDOWEVENT_RESTORED,

    BP_WINDOWEVENT_ENTER = SDL_WINDOWEVENT_ENTER,
    BP_WINDOWEVENT_LEAVE = SDL_WINDOWEVENT_LEAVE,
    BP_WINDOWEVENT_FOCUS_GAINED = SDL_WINDOWEVENT_FOCUS_GAINED,
    BP_WINDOWEVENT_FOCUS_LOST = SDL_WINDOWEVENT_FOCUS_LOST,
    BP_WINDOWEVENT_CLOSE = SDL_WINDOWEVENT_CLOSE
} BP_WindowEventID;

typedef enum
{
    BPK_LEFT = SDLK_LEFT,
    BPK_RIGHT = SDLK_RIGHT,
    BPK_UP = SDLK_UP,
    BPK_DOWN = SDLK_DOWN,
    BPK_SPACE = SDLK_SPACE,
    BPK_ESCAPE = SDLK_ESCAPE,
    BPK_RETURN = SDLK_RETURN,
    BPK_DELETE = SDLK_DELETE,
    BPK_BACKSPACE = SDLK_BACKSPACE
} BP_KeyBoard;

typedef enum
{
    BP_BUTTON_LEFT = SDL_BUTTON_LEFT,
    BP_BUTTON_MIDDLE = SDL_BUTTON_MIDDLE,
    BP_BUTTON_RIGHT = SDL_BUTTON_RIGHT
} BP_Button;

//mingw无std::mutex
#ifdef __MINGW32__
class mutex
{
private:
    SDL_mutex* _mutex;
public:
    mutex() { _mutex = SDL_CreateMutex(); }
    ~mutex() { SDL_DestroyMutex(_mutex); }
    int lock() { return SDL_LockMutex(_mutex); }
    int unlock() { return SDL_UnlockMutex(_mutex); }
};
#endif
