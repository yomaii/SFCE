#include "2d.h"
SDL_Window* window = NULL;
SDL_Surface* surface = NULL;
uint32_t bg_data[256 * 256 + 256];
uint32_t palette_data[16];

int key_map[] = {
    SDLK_j,
    SDLK_k,
    SDLK_u,
    SDLK_i,
    SDLK_w,
    SDLK_s,
    SDLK_a,
    SDLK_d
};

uint32_t GetPixel(unsigned x, unsigned y, const uint8_t* nt, const uint8_t* bg) {
    // 获取所在名称表

    const unsigned id = (x >> 3) + (y >> 3) * 32;

    const uint32_t name = nt[id];
    // 查找对应图样表
    const uint8_t* nowp0 = bg + name * 16;
    const uint8_t* nowp1 = nowp0 + 8;

    // Y坐标为平面内偏移
    const int offset = y & 0x7;
    const uint8_t p0 = nowp0[offset];
    const uint8_t p1 = nowp1[offset];

    // X坐标为字节内偏移
    const uint8_t shift = (~x) & 0x7;
    const uint8_t mask = 1 << shift;
    // 计算低二位
    const uint8_t low = ((p0 & mask) >> shift) | ((p1 & mask) >> shift << 1);
    // 计算所在属性表
    const unsigned aid = (x >> 5) + (y >> 5) * 8;
    const uint8_t attr = nt[aid + (32*30)];
    // 获取属性表内位偏移
    const uint8_t aoffset = ((x & 0x10) >> 3) | ((y & 0x10) >> 2);
    // 计算高两位
    const uint8_t high = (attr & (3 << aoffset)) >> aoffset << 2;
    // 合并作为颜色
    const uint8_t index = high | low;

    return palette_data[index];
}
extern void MainRender(uint32_t* rgba) {
    uint32_t* data = rgba;
    for (int i = 0; i != 10000; ++i)
        famicom.cpu_->ExecuteOne();

    famicom.sVblank();
    if (famicom.ppu_.ctrl & (uint8_t)PPU2000_NMIGen) {
        famicom.cpu_->NMI();
    }

    // 生成调色板颜色
    {
        for (int i = 0; i != 16; ++i) {
            palette_data[i] = palette[famicom.ppu_.spindexes[i]].data;
        }
        palette_data[4 * 1] = palette_data[0];
        palette_data[4 * 2] = palette_data[0];
        palette_data[4 * 3] = palette_data[0];
    }
    // 背景
    const uint8_t* now = famicom.ppu_.banks[8];
    const uint8_t* bgp = famicom.ppu_.banks[
        famicom.ppu_.ctrl & PPU2000_BgTabl ? 4 : 0];
    for (unsigned i = 0; i != 256 * 240; ++i) {
        data[i] = GetPixel(i & 0xff, i >> 8, now, bgp);
    }

}
void UserInput(int index, unsigned char data){
    assert(index >= 0 && index < 16);
    famicom.controller_states_[index] = data;
}
void CreateWindow(){
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("SDL", 100, 100, 256, 240, SDL_WINDOW_SHOWN);
    surface = SDL_GetWindowSurface(window);
    bool quit = false;
    SDL_Event e;
    while(!quit){
        for(int i=0;i<8;i++){
            UserInput(i, 0);
        }
        while( SDL_PollEvent( &e ) != 0 ){
            if(e.type == SDL_QUIT) quit = true;
            if(e.type == SDL_KEYDOWN){
                for(int i=0;i<8;i++){
                    UserInput(i, key_map[i] == e.key.keysym.sym);
                }
            }
        }
        uint32_t color = SDL_MapRGB(surface->format, 0xff, 0, 0);
        uint32_t* pixel = (uint32_t*)surface->pixels;
        MainRender(bg_data);
        for(int i = 0; i < 256*240; i++)
            pixel[i] = bg_data[i];
        SDL_UnlockSurface(surface);
        SDL_UpdateWindowSurface(window);
    }
    SDL_Quit();
}