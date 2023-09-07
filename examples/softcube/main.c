#include <inttypes.h>
#include <brender.h>
#include <brddi.h>
#include <priminfo.h>
#include <brsdl2dev.h>
#include <stdio.h>
#include <SDL.h>
#include <assert.h>

void BR_CALLBACK _BrBeginHook(void)
{
    struct br_device *BR_EXPORT BrDrv1SoftPrimBegin(const char *arguments);
    struct br_device *BR_EXPORT BrDrv1SoftRendBegin(const char *arguments);

    BrDevAddStatic(NULL, BrDrv1SoftPrimBegin, NULL);
    BrDevAddStatic(NULL, BrDrv1SoftRendBegin, NULL);
    BrDevAddStatic(NULL, BrDrv1SDL2Begin, NULL);
}

void BR_CALLBACK _BrEndHook(void)
{
}

static char primitive_heap[1500 * 1024];

static void draw_info(br_pixelmap *screen, br_material *mat)
{
    br_token   pstate_token = BR_NULL_TOKEN;
    br_object *primstate    = NULL;
    br_uint_16 font_height  = 0;
    brp_block *block;

    if((pstate_token = BrTokenFind("PRIMITIVE_STATE_O")) == BR_NULL_TOKEN)
        return;

    // ObjectQuery(BrV1dbRendererQuery(), &primstate, pstate_token);
    ObjectQuery(mat->stored, &primstate, pstate_token);
    if(primstate == NULL)
        return;

    ObjectQuery(primstate, &block, BRT_PRIMITIVE_BLOCK_P);
    if(block == NULL)
        return;

    font_height = BrPixelmapTextHeight(screen, BrFontProp7x9);

    BrPixelmapTextF(screen, -(screen->width / 2) + font_height, -(screen->height / 2) + font_height, 0xFFFFFFFF,
                    BrFontProp7x9, "Rasteriser: %s", block->identifier);
}

int main(int argc, char **argv)
{
    br_pixelmap *screen = NULL, *colour_buffer = NULL, *depth_buffer = NULL;
    br_actor    *world, *camera, *cube, *light;
    int          ret = 1;
    br_uint_64   ticks_last, ticks_now;
    br_colour    clear_colour;
    br_error     err;

#if 0 // defined(_DEBUG)
    if(!IsDebuggerPresent())
        MessageBoxA(NULL, "ATTACH DEBUGGER NOW", "ATTACH DEBUGGER NOW", MB_OK);
#endif

    BrBegin();

    BrLogSetLevel(BR_LOG_DEBUG);

    err = BrDevBeginVar(&screen, "SDL2", BRT_WIDTH_I32, 1280, BRT_HEIGHT_I32, 720, BR_NULL_TOKEN);

    if(err != BRE_OK) {
        BrDevBeginVar(NULL, "BrDevBeginVar() failed", "BRender Error", MB_OK | MB_ICONERROR);
        goto create_fail;
    }

    {
#if defined(SOFTCUBE_16BIT)
        BrLogInfo("APP", "Running at 16-bpp");
        colour_buffer = BrPixelmapMatchTyped(screen, BR_PMMATCH_OFFSCREEN, BR_PMT_RGB_565);
        ;
        clear_colour = BR_COLOUR_565(66, 66, 66);

#else
        BrLogInfo("APP", "Running at 24-bpp");
        colour_buffer = BrPixelmapMatchTyped(screen, BR_PMMATCH_OFFSCREEN, BR_PMT_RGB_888);
        ;
        clear_colour = BR_COLOUR_RGB(66, 66, 66);
#endif
    }

    if(colour_buffer == NULL) {
        MessageBoxA(NULL, "BrPixelmapAllocate() failed", "BRender Error", MB_OK | MB_ICONERROR);
        goto create_fail;
    }

    if((depth_buffer = BrPixelmapMatch(colour_buffer, BR_PMMATCH_DEPTH_16)) == NULL) {
        MessageBoxA(NULL, "BrPixelmapMatch() failed", "BRender Error", MB_OK | MB_ICONERROR);
        goto create_fail;
    }

    colour_buffer->origin_x = depth_buffer->origin_x = colour_buffer->width >> 1;
    colour_buffer->origin_y = depth_buffer->origin_y = colour_buffer->height >> 1;

    BrRendererBegin(colour_buffer, NULL, NULL, primitive_heap, sizeof(primitive_heap));

    world = BrActorAllocate(BR_ACTOR_NONE, NULL);

    {
        br_camera *camera_data;

        camera         = BrActorAdd(world, BrActorAllocate(BR_ACTOR_CAMERA, NULL));
        camera->t.type = BR_TRANSFORM_MATRIX34;
        BrMatrix34Translate(&camera->t.t.mat, BR_SCALAR(0.0), BR_SCALAR(0.0), BR_SCALAR(5.0));

        camera_data         = (br_camera *)camera->type_data;
        camera_data->aspect = BR_DIV(BR_SCALAR(colour_buffer->width), BR_SCALAR(colour_buffer->height));
    }

    BrModelFindHook(BrModelFindFailedLoad);
    BrMapFindHook(BrMapFindFailedLoad);
    BrMaterialFindHook(BrMaterialFindFailedLoad);

    cube         = BrActorAdd(world, BrActorAllocate(BR_ACTOR_MODEL, NULL));
    cube->t.type = BR_TRANSFORM_MATRIX34;
    cube->model  = BrModelFind("cube.dat");

#if defined(SOFTCUBE_16BIT)
    cube->material = BrMaterialLoad("checkerboard8.mat");
#else
    cube->material = BrMaterialLoad("checkerboard24.mat");
#endif

    cube->material->flags |= BR_MATF_PERSPECTIVE; // Perspective-correct texture mapping. Doesn't actually work.
    cube->material->flags |= BR_MATF_DITHER;      // Dithering.
    cube->material->flags |= BR_MATF_SMOOTH;      // Makes lighting look _much_ better.
    // cube->material->flags |= BR_MATF_DISABLE_COLOUR_KEY;  // Not supported by software.
    cube->material->opacity = 255; // < 255 selects screendoor renderer

    BrMapUpdate(cube->material->colour_map, BR_MAPU_ALL);
    BrMaterialUpdate(cube->material, BR_MATU_ALL);

    BrMatrix34RotateY(&cube->t.t.mat, BR_ANGLE_DEG(30));

    light = BrActorAdd(world, BrActorAllocate(BR_ACTOR_LIGHT, NULL));
    BrLightEnable(light);

    ticks_last = SDL_GetTicks64();

    for(SDL_Event evt;;) {
        float dt;

        ticks_now  = SDL_GetTicks64();
        dt         = (float)(ticks_now - ticks_last) / 1000.0f;
        ticks_last = ticks_now;

        while(SDL_PollEvent(&evt) > 0) {
            switch(evt.type) {
                case SDL_QUIT:
                    goto done;
            }
        }

        BrMatrix34PostRotateY(&cube->t.t.mat, BR_ANGLE_DEG(BR_SCALAR(50) * BR_SCALAR(dt)));

        BrRendererFrameBegin();
        BrPixelmapFill(colour_buffer, clear_colour);
        BrPixelmapFill(depth_buffer, 0xFFFFFFFF);
        BrZbSceneRender(world, camera, colour_buffer, depth_buffer);
        BrRendererFrameEnd();

        draw_info(colour_buffer, cube->material);

        BrPixelmapDoubleBuffer(screen, colour_buffer);
    }

done:
    ret = 0;

    BrRendererEnd();

create_fail:

    if(depth_buffer != NULL)
        BrPixelmapFree(depth_buffer);

    if(colour_buffer != NULL)
        BrPixelmapFree(colour_buffer);

    BrEnd();

    return ret;
}
