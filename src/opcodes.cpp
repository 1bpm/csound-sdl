#include <plugin.h>
#include <SDL2/SDL.h>


struct SDLColour {
    int r;
    int g;
    int b;
    int a;
};

struct SDLSession {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    int screen_width;
    int screen_height;
    bool initialised = false;
    bool cycle = false;
};

const char* sessionName = "::sdlsession%d";
const char* colourName = "::sdlcolour%d";
const char* badHandle = "cannot obtain data from handle";

/*
 * Create connection in global variables returning handle
 */

MYFLT createHandle(csnd::Csound* csound, SDLSession** sdl) {
    char buffer[32];
    int handle = 0;
    snprintf(buffer, 32, sessionName, handle);
    while ((*sdl = (SDLSession*) csound->query_global_variable(buffer)) != NULL) {
        snprintf(buffer, 32, sessionName, ++handle);
    }
    csound->create_global_variable(buffer, sizeof(SDLSession));
    *sdl = (SDLSession*) csound->query_global_variable(buffer);
    
    return FL(handle);
}

MYFLT CreateColour(csnd::Csound* csound, SDLColour** sdlc) {
    char buffer[32];
    int handle = 0;
    snprintf(buffer, 32, colourName, handle);
    while ((*sdlc = (SDLColour*) csound->query_global_variable(buffer)) != NULL) {
        snprintf(buffer, 32, colourName, ++handle);
    }
    csound->create_global_variable(buffer, sizeof(SDLColour));
    *sdlc = (SDLColour*) csound->query_global_variable(buffer);
    
    return FL(handle);
}

SDLSession* getSession(csnd::Csound* csound, MYFLT handle) {
    char buffer[32];
    snprintf(buffer, 32, sessionName, (int)handle);
    return (SDLSession*) csound->query_global_variable(buffer);  
}

SDLColour* getColour(csnd::Csound* csound, MYFLT handle) {
    char buffer[32];
    snprintf(buffer, 32, colourName, (int)handle);
    return (SDLColour*) csound->query_global_variable(buffer);  
}

// ihandle, kmouseTrigger, kmouseXratio, kmouseYratio sdlinit SwindowName, iWidth, iHeight, ifps
struct sdlinit : csnd::Plugin<4, 4> {
    static constexpr char const *otypes = "ikkk";
    static constexpr char const *itypes = "Siii";
    SDLSession* sdl;
    uint64_t kcnt;
    uint64_t frameK;
    bool mouseDown;
    
    int init() {
        csound->plugin_deinit(this);
        outargs[0] = createHandle(csound, &sdl);
        mouseDown = false;

        STRINGDAT &windowName = inargs.str_data(0);
        int xSize = (int) inargs[1];
        int ySize = (int) inargs[2];
        int fps = (int) inargs[3];

        try {
            init_sdl(windowName.data, xSize, ySize);
        } catch (const std::exception &e) {
            return csound->init_error(e.what());
        }
        
        kcnt = insdshead->kcounter;
        frameK = (uint64_t) (FL(FL(1)/FL(fps)) / insdshead->onedkr);
        
        return OK;
    }
    
    void init_sdl(char* windowName, int xSize, int ySize) {
        if (sdl->initialised) {
            return;
        }
        sdl->screen_width = xSize;
        sdl->screen_height = ySize;
        
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            throw std::runtime_error(csound->strdup((char*) SDL_GetError()));
        }
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
        sdl->window = SDL_CreateWindow(windowName, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, xSize, ySize, SDL_WINDOW_SHOWN );
        if (sdl->window == NULL) {
            throw std::runtime_error(csound->strdup((char*) SDL_GetError()));
        }
        
        sdl->renderer = SDL_CreateRenderer(sdl->window, -1, SDL_RENDERER_ACCELERATED);
        if (sdl->renderer == NULL) {
            throw std::runtime_error(csound->strdup((char*) SDL_GetError()));
        }
        sdl->initialised = true;
    }
    
    int deinit() {
        SDL_DestroyRenderer(sdl->renderer);
	SDL_DestroyWindow(sdl->window);
	sdl->window = NULL;
	sdl->renderer = NULL;
        sdl->initialised = false;
	SDL_Quit();
        return OK;
    }
    
    void setMouse() {
        int x;
        int y;
        SDL_GetMouseState(&x, &y);
        outargs[1] = 1;
        outargs[2] = (MYFLT) (FL(x) / FL(sdl->screen_width));
        outargs[3] = (MYFLT) (FL(y) / FL(sdl->screen_height));
    }
    
    int kperf() {
        if (insdshead->kcounter % frameK != 0) {
            sdl->cycle = false;
            return OK;
        }
        SDL_Event e;
        outargs[1] = 0;
        outargs[2] = 0;
        outargs[3] = 0;
        while (SDL_PollEvent(&e) != 0) {
            // e.type == SDL_MOUSEMOTION || 
            // || e.type == SDL_MOUSEBUTTONUP
            if (e.type == SDL_MOUSEBUTTONDOWN || (mouseDown && e.type == SDL_MOUSEMOTION)) {
                setMouse();
                mouseDown = true;
            }
            if (e.type == SDL_MOUSEBUTTONUP) {
                mouseDown = false;
            }
        }
        
        sdl->cycle = true;
        SDL_RenderPresent(sdl->renderer);
        SDL_SetRenderDrawColor(sdl->renderer, 0xFF, 0xFF, 0xFF, 0xFF );
        SDL_RenderClear(sdl->renderer);
        return OK;
    }
};

// ihandle sdlcolour red, green, blue, alpha
struct sdlcolour : csnd::Plugin<1, 4> {
    static constexpr char const *otypes = "i";
    static constexpr char const *itypes = "kkkk";
    SDLColour* sdlc;
    
    int init() {
        outargs[0] = CreateColour(csound, &sdlc);
        return OK;
    }
    
    int kperf() {
        sdlc->r = (int) inargs[0];
        sdlc->g = (int) inargs[1];
        sdlc->b = (int) inargs[2];
        sdlc->a = (int) inargs[3];
        return OK;
    }
};


// sdlrect ihandle, icolourhandle, kx, ky, kwidth, kheight
struct sdlrect : csnd::InPlug<6> {
    static constexpr char const *otypes = "";
    static constexpr char const *itypes = "iikkkk";
    SDLSession* sdl;
    SDLColour* sdlc;
    
    
    int init() {
        if (!(sdl = getSession(csound, args[0]))) {
            return csound->init_error(badHandle);
        }
        
        if (!(sdlc = getColour(csound, args[1]))) {
            return csound->init_error(badHandle);
        }
        
        return OK;
    }
    
    int kperf() {
        if (!sdl->cycle) {
            return OK;
        }

        int xpos = (int) sdl->screen_width * args[2];
        int ypos = (int) sdl->screen_height * args[3];
        int width = (int) sdl->screen_width * args[4];
        int height = (int) sdl->screen_height * args[5];
  
        SDL_Rect fillRect = { 
            xpos, ypos, width, height
        };

        SDL_SetRenderDrawColor(sdl->renderer, sdlc->r, sdlc->g, sdlc->b, sdlc->a);				
        SDL_RenderFillRect(sdl->renderer, &fillRect);

        return OK;
    }
};


struct sdlline : csnd::InPlug<6> {
    static constexpr char const *otypes = "";
    static constexpr char const *itypes = "iikkkk";
    SDLSession* sdl;
    SDLColour* sdlc;
    
    
    int init() {
        if (!(sdl = getSession(csound, args[0]))) {
            return csound->init_error(badHandle);
        }
        
        if (!(sdlc = getColour(csound, args[1]))) {
            return csound->init_error(badHandle);
        }
        
        return OK;
    }
    
    int kperf() {
        if (!sdl->cycle) {
            return OK;
        }

        int xpos1 = (int) sdl->screen_width * args[2];
        int ypos1 = (int) sdl->screen_height * args[3];
        int xpos2 = (int) sdl->screen_width * args[4];
        int ypos2 = (int) sdl->screen_height * args[5];

        SDL_SetRenderDrawColor(sdl->renderer, sdlc->r, sdlc->g, sdlc->b, sdlc->a);				
        SDL_RenderDrawLine(sdl->renderer, xpos1, ypos1, xpos2, ypos2);

        return OK;
    }
};



#include <modload.h>

void csnd::on_load(csnd::Csound *csound) {

    csnd::plugin<sdlinit>(csound, "sdlinit", csnd::thread::ik);
    csnd::plugin<sdlrect>(csound, "sdlrect", csnd::thread::ik);
    csnd::plugin<sdlcolour>(csound, "sdlcolour", csnd::thread::ik);
    csnd::plugin<sdlline>(csound, "sdlline", csnd::thread::ik);
}
