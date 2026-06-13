#include "NWengineCore.h"

Texture texture;

extern void raster_init();
extern void raster_update();
extern void raster_destroy(); 
extern void* get_raw_framebuffer();

void init() {
    texture._size.x = 512; texture._size.y = 512;
    texture._GPUGen(0, TexChannelInfo::NW_RGBA, TexType_Exp::TexType_Exp_rgba8);
    Scene& scene = Scene::CreateNew("NewScene");
    scene.MakeCurrent();
    GameObject& cam_obj = scene.AddObject("cam_obj");
    GameObject& quad = scene.AddObject("quad"); 
    Camera&     cam  = cam_obj.Add<Camera>();
    quad.Add<Sprite,Transform>();
    cam.ChangeOrtho(512, 512);
    cam.Use();
    cam.clearColor = {1.,1.,0.};
    scene.Start();
    raster_init();
}

int x;
int s = 1;

void render() {
    GameObject* obj = Scene::GetCurrent()->GetGameObject("quad");
    raster_update();
    texture.SetFromCPU((ui8*)get_raw_framebuffer(), TexChannelInfo::NW_RGBA, TexType_Exp::TexType_Exp_rgba16f);
    Sprite*     spr = obj->Get<Sprite>();
    raster_update();
    Vector3<ui8> c;
    c.x = 255;
    c.z = x;
    spr->SetTexture(&texture);
    (*Renderer::currentRenderer)(1);
}

int main() { 
    NWenginePushFunction(ON_MAIN_CALL_LOCATION::InitEnd, init);
    NWenginePushFunction(ON_MAIN_CALL_LOCATION::FrameIntermediate, render); 
    NWengineInit();
    NWengineLoop();
    NWengineShutdown();
    raster_destroy();
    return 0;     
}
