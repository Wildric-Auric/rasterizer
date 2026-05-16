#include "NWengineCore.h"
#include <cstring>

ui8*   fmb;
Texture texture;

void fill_fmb(ui8* fmb, const v2i&, const Vector3<ui8>& c) {
    for (int i = 0; i < 512*512*4; i+=4) {
        fmb[i]   = c.x; 
        fmb[i+1] = c.y; 
        fmb[i+2] = c.z; 
        fmb[i+3] = 255;
    }
}

void init() {
    fmb = (ui8*)malloc(512*512*4);
    fill_fmb(fmb, {512,512}, {255,0,255});
    texture._size.x = 512; texture._size.y = 512;
    texture._GPUGen(fmb, TexChannelInfo::NW_RGBA, TexType_Exp::TexType_Exp_rgba16f);
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
}

int x;
void render() {
    GameObject* obj = Scene::GetCurrent()->GetGameObject("quad");
    texture.SetFromCPU(fmb, TexChannelInfo::NW_RGBA, TexType_Exp::TexType_Exp_rgba16f);
    Sprite*     spr = obj->Get<Sprite>();
    Vector3<ui8> c;
    c.x = 255;
    c.z = x;
    fill_fmb(fmb, {255,255}, c);
    ++x; if (x > 255) x = 0;
    spr->SetTexture(&texture);
    (*Renderer::currentRenderer)(1);
}

int main() { 
    NWenginePushFunction(ON_MAIN_CALL_LOCATION::InitEnd, init);
    NWenginePushFunction(ON_MAIN_CALL_LOCATION::FrameIntermediate, render); 
    NWengineInit();
    NWengineLoop();
    NWengineShutdown();
    free(fmb);
    return 0;     
}
