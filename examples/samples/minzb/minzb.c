/*
 * Copyright (c) 1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id:  $
 * $Locker:  $
 *
 */

#include "brender.h"
#include "dosio.h"

/*
 * The screen, offscreen buffer, and the depth buffer
 */
br_pixelmap *screen_buffer, *back_buffer, *depth_buffer;

/*
 * The actors in the world
 */
br_actor *observer, *world;

int main(int argc, char **argv)
{
    br_actor *a;
    int       i;

    /*
     * Setup renderer and screen
     */
    BrBegin();
    InitializeSampleZBuffer(&screen_buffer, &back_buffer, &depth_buffer);

    /*
     * Build the a world
     */
    world    = BrActorAllocate(BR_ACTOR_NONE, NULL);
    observer = CreateSampleCamera(world);
    BrLightEnable(BrActorAdd(world, BrActorAllocate(BR_ACTOR_LIGHT, NULL)));

    a                    = BrActorAdd(world, BrActorAllocate(BR_ACTOR_MODEL, NULL));
    a->t.type            = BR_TRANSFORM_EULER;
    a->t.t.euler.e.order = BR_EULER_ZXY_S;
    BrVector3Set(&a->t.t.euler.t, BR_SCALAR(0.0), BR_SCALAR(0.0), BR_SCALAR(-5.0));

    /*
     * Tumble the actor around
     */
    float dt;
    i = 0;
    while(UpdateSample(observer, &dt)) {
        /*
         * Clear the buffers
         */
        BrPixelmapFill(back_buffer, 0);
        BrPixelmapFill(depth_buffer, 0xFFFFFFFF);

        BrZbSceneRender(world, observer, back_buffer, depth_buffer);
        BrPixelmapDoubleBuffer(screen_buffer, back_buffer);

        a->t.t.euler.e.a = BR_ANGLE_DEG(i);
        a->t.t.euler.e.b = BR_ANGLE_DEG(i * 2);
        a->t.t.euler.e.c = BR_ANGLE_DEG(i * 3);

        i++;
        if(i >= 360)
            i = 0;
    }

    /*
     * Close down
     */
    BrZbEnd();
    DOSGfxEnd();
    BrEnd();

    return 0;
}
