#include "NWengineCore.h"

Texture texture;

extern void ras_init();
extern void ras_update();
extern void ras_destroy(); 
extern void* ras_get_raw_framebuffer();

void ras_backend_get_mouse(int* x, int* y) {
    fVec2 v = Inputs::GetMousePosition(); 
    *x = v.x + (texture._size.x >> 1);
    *y = v.y + (texture._size.y >> 1);
}

void ras_backend_resize(int new_size_x, int new_size_y) {
    if (texture._size.x == new_size_x && texture._size.y != new_size_y)
        return;
    if (texture._glID)
        texture.Destroy();
    texture._size.x = new_size_x; texture._size.y = new_size_y;
    texture._GPUGen(0, TexChannelInfo::NW_RGBA, TexType_Exp::TexType_Exp_rgba8);
    Scene::currentScene->GetGameObject("cam_obj")->Get<Camera>()->ChangeOrtho(new_size_x, new_size_y);
}

void init() {
    Scene& scene = Scene::CreateNew("NewScene");
    scene.MakeCurrent();
    GameObject& cam_obj = scene.AddObject("cam_obj");
    GameObject& quad = scene.AddObject("quad"); 
    Camera&     cam  = cam_obj.Add<Camera>();
    quad.Add<Sprite,Transform>();
    cam.Use();
    cam.clearColor = {1.,1.,0.};
    scene.Start();
    ras_init();
}

int x;
int s = 1;

void render() {
    GameObject* obj = Scene::GetCurrent()->GetGameObject("quad");
    ras_update();
    texture.SetFromCPU((ui8*)ras_get_raw_framebuffer(), TexChannelInfo::NW_RGBA, TexType_Exp::TexType_Exp_rgba16f);
    Sprite*     spr = obj->Get<Sprite>();
    ras_update();
    spr->SetTexture(&texture);
    (*Renderer::currentRenderer)(1);
}

int main() { 
    NWenginePushFunction(ON_MAIN_CALL_LOCATION::InitEnd, init);
    NWenginePushFunction(ON_MAIN_CALL_LOCATION::FrameIntermediate, render); 
    NWengineInit();
    NWengineLoop();
    NWengineShutdown();
    ras_destroy();
    return 0;     
}
