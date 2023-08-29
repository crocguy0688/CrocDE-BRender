
/*
 * Copyright (c) 1993 Argonaut Software Ltd. All rights reserved.
 */

/* Teapot in Space */
/* Simulated reflections by rendering into environment maps */

#include <stdio.h>
#include <stdlib.h>

#include "brender.h"
#include "dosio.h"

/* This demo simulates a giant reflecting teapot hanging above a */
/* spinning earth */

/* Firstly `mirror' is the structure defining the material from which */
/* the teapot is made...an environment map */

br_material _mirror = {"mirror",
                       BR_COLOUR_RGB(255, 255, 255),
                       255,
                       BR_UFRACTION(0.05),
                       BR_UFRACTION(0.45),
                       BR_UFRACTION(0.50),
                       BR_SCALAR(20),

                       /* The teapot model is subdivided into many small triangles. This */
                       /* means we can use BR_MATF_ENVIRONMENT_I even though it is */
                       /* less accurate than BR_MATF_ENVIRONMENT_L */

                       BR_MATF_GOURAUD | BR_MATF_LIGHT | BR_MATF_ENVIRONMENT_L,
                       {{BR_VECTOR2(1, 0), BR_VECTOR2(0, 1), BR_VECTOR2(0, 0)}},
                       0,
                       63};

/* Now the material from which the earth is made. */

br_material _earth_map = {"earth",
                          BR_COLOUR_RGB(255, 255, 255),
                          255,
                          BR_UFRACTION(0.05),
                          BR_UFRACTION(0.45),
                          BR_UFRACTION(0.50),
                          BR_SCALAR(20),
                          BR_MATF_GOURAUD | BR_MATF_LIGHT,
                          {{BR_VECTOR2(1, 0), BR_VECTOR2(0, 1), BR_VECTOR2(0, 0)}},
                          0,
                          63};

br_material _moon_map = {"moon",
                         BR_COLOUR_RGB(255, 255, 255),
                         255,
                         BR_UFRACTION(0.05),
                         BR_UFRACTION(0.45),
                         BR_UFRACTION(0.50),
                         BR_SCALAR(20),
                         BR_MATF_GOURAUD | BR_MATF_LIGHT,
                         {{BR_VECTOR2(1, 0), BR_VECTOR2(0, 1), BR_VECTOR2(0, 0)}},
                         0,
                         63};

int RotateTeapot = 0;

br_pixelmap *palette;

/*
 * Find Failed callbacks to automatically load textures & tables
 */
br_pixelmap *BR_CALLBACK MapFindFailedLoad(const char *name)
{
    br_pixelmap *pm;

    if((pm = BrPixelmapLoad(name)) != NULL) {
        pm->identifier = BrResStrDup(pm, name);
        if(pm->type == BR_PMT_INDEX_8) {
            pm->map = palette;
            pm      = BrPixelmapDeCLUT(pm);
        }
        BrMapAdd(pm);
    }

    return pm;
}

int main(void)
{
    br_actor    *world, *teapot, *earth, *moon, *camera, *mirror_view, *light1, *env;
    br_actor    *pair, *system, *light2;
    br_pixelmap *depth_buffer, *mirror_depth, *shade, *earth_pm, *moon_pm;
    br_pixelmap *colour_buffer, *mirror_pm, *envmap1_pm, *screen;
    br_colour   *pal_entry;
    br_matrix34  rotation;
    br_material *moon_map, *earth_map, *mirror;
    br_angle     theta = 0;

    int i;

    long          mouse_x, mouse_y;
    unsigned long mouse_buttons;

    BR_BANNER("Reflection Demo", "1994-1995", "$Revision: 1.2 $");

    /* Initialise BRender */
    InitializeSampleZBuffer(&screen, &colour_buffer, &depth_buffer);
    palette = BrPixelmapLoad("std.pal");

    BrTableFindHook(BrTableFindFailedLoad);
    BrMapFindHook(MapFindFailedLoad);
    BrModelFindHook(BrModelFindFailedLoad);
    BrTableFindHook(BrTableFindFailedLoad);

    /* Load a shade table */

    shade = BrPixelmapLoad("shade.tab");
    if(shade == NULL)
        BR_ERROR0("Could't load shade.tab");

    /* Put shade table into registry */

    BrTableAdd(shade);

    /* Complete definition of mirror texture. Note that the colour_map */
    /* is a newly allocated pixelmap into which we'll render later */

    mirror              = BrMaterialAllocate("mirror");
    *mirror             = _mirror;
    mirror->index_shade = shade;
    mirror->colour_map = mirror_pm = BrPixelmapAllocate(screen->type, 256, 256, NULL, 0);
    BrMapAdd(mirror_pm);
    BrMaterialAdd(mirror);

    mirror_depth = BrPixelmapMatch(mirror_pm, BR_PMMATCH_DEPTH_16);

    /* The earth texture is now defined using a predefined earth texture */

    earth_map     = BrMaterialAllocate("earth");
    *earth_map    = _earth_map;
    earth_pm      = BrPixelmapLoad("earth.pix");
    earth_pm->map = palette;
    earth_pm      = BrPixelmapDeCLUT(earth_pm);
    if(earth_pm == NULL)
        BR_ERROR0("Couldn't load earth.pix");
    BrMapAdd(earth_pm);
    earth_map->index_shade = shade;
    earth_map->colour_map  = earth_pm;
    BrMaterialAdd(earth_map);

    moon_map     = BrMaterialAllocate("moon");
    *moon_map    = _moon_map;
    moon_pm      = BrPixelmapLoad("moon.pix");
    moon_pm->map = palette;
    moon_pm      = BrPixelmapDeCLUT(moon_pm);
    if(moon_pm == NULL)
        BR_ERROR0("Couldn't load moon.pix");
    BrMapAdd(moon_pm);
    moon_map->index_shade = shade;
    moon_map->colour_map  = moon_pm;
    BrMaterialAdd(moon_map);

    /* If the teapot simply reflected the earth with no other source */
    /* for reflection then it would mostly look black so rather than simply */
    /* reflect the earth into the teapot we'll also have an environment map */
    /* to provide a plain background that is also reflected */

    envmap1_pm      = BrPixelmapLoad("testenv1.pix");
    envmap1_pm->map = palette;
    envmap1_pm      = BrPixelmapDeCLUT(envmap1_pm);
    if(envmap1_pm == NULL)
        BR_ERROR0("Couldn't load testenv1.pix");
    BrMapAdd(envmap1_pm);

    /* In the beginning... */

    world = BrActorAllocate(BR_ACTOR_NONE, NULL);

    /* The earth-teapot system forms a single `pair' unit */

    pair         = BrActorAdd(world, BrActorAllocate(BR_ACTOR_NONE, NULL));
    pair->t.type = BR_TRANSFORM_MATRIX34;
    BrMatrix34Identity(&pair->t.t.mat);

    system         = BrActorAdd(pair, BrActorAllocate(BR_ACTOR_NONE, NULL));
    system->t.type = BR_TRANSFORM_MATRIX34;
    BrMatrix34PostTranslate(&system->t.t.mat, BR_SCALAR(-0.7), BR_SCALAR(0), BR_SCALAR(0));

    /* Load teapot and place into `pair' */

    teapot        = BrActorAdd(pair, BrActorAllocate(BR_ACTOR_MODEL, NULL));
    teapot->model = BrModelLoad("teapot.dat");
    if(teapot->model == NULL)
        BR_ERROR0("Couldn't load teapot.dat");
    BrModelAdd(teapot->model);
    teapot->material = BrMaterialFind("mirror");
    teapot->t.type   = BR_TRANSFORM_MATRIX34;
    BrMatrix34Scale(&teapot->t.t.mat, BR_SCALAR(0.7), BR_SCALAR(0.7), BR_SCALAR(0.7));
    BrMatrix34PostRotateX(&teapot->t.t.mat, BR_ANGLE_DEG(-70));
    BrMatrix34PostRotateY(&teapot->t.t.mat, BR_ANGLE_DEG(-45));
    BrMatrix34PostTranslate(&teapot->t.t.mat, BR_SCALAR(0.7), BR_SCALAR(0), BR_SCALAR(0));

    /* Load and place spheres into earth-moon system */

    earth        = BrActorAdd(system, BrActorAllocate(BR_ACTOR_MODEL, NULL));
    earth->model = BrModelLoad("sph32.dat");
    if(earth->model == NULL)
        BR_ERROR0("Couldn't load sph32.dat");
    BrModelAdd(earth->model);
    earth->material = BrMaterialFind("earth");
    earth->t.type   = BR_TRANSFORM_MATRIX34;

    moon           = BrActorAdd(system, BrActorAllocate(BR_ACTOR_MODEL, NULL));
    moon->model    = earth->model;
    moon->material = BrMaterialFind("moon");
    moon->t.type   = BR_TRANSFORM_MATRIX34;

    /* Now place the main camera from which we will view the system */

    camera                                          = CreateSampleCamera(world);
    ((br_camera *)camera->type_data)->type          = BR_CAMERA_PERSPECTIVE;
    ((br_camera *)camera->type_data)->field_of_view = BR_ANGLE_DEG(45.0);
    ((br_camera *)camera->type_data)->hither_z      = BR_SCALAR(0.1);
    ((br_camera *)camera->type_data)->yon_z         = BR_SCALAR(20.0);
    ((br_camera *)camera->type_data)->aspect        = BR_SCALAR(1.46);
    camera->t.type                                  = BR_TRANSFORM_MATRIX34;
    BrMatrix34Translate(&camera->t.t.mat, BR_SCALAR(0.0), BR_SCALAR(0.0), BR_SCALAR(2.0));

    /* This is the camera that will be used to provide the reflection */
    /* of the earth into the teapot */

    mirror_view                                          = BrActorAdd(pair, BrActorAllocate(BR_ACTOR_CAMERA, NULL));
    ((br_camera *)mirror_view->type_data)->type          = BR_CAMERA_PERSPECTIVE;
    ((br_camera *)mirror_view->type_data)->field_of_view = BR_ANGLE_DEG(90.0);
    ((br_camera *)mirror_view->type_data)->hither_z      = BR_SCALAR(0.1);
    ((br_camera *)mirror_view->type_data)->yon_z         = BR_SCALAR(20.0);
    ((br_camera *)mirror_view->type_data)->aspect        = BR_SCALAR(1.46);
    mirror_view->t.type                                  = BR_TRANSFORM_MATRIX34;

    /* Rotate the camera to face the earth... */

    BrMatrix34RotateY(&mirror_view->t.t.mat, BR_ANGLE_DEG(90));

    /* ...and move it into the centre of the teapot */

    BrMatrix34PostTranslate(&mirror_view->t.t.mat, BR_SCALAR(0.5), BR_SCALAR(0), BR_SCALAR(0));

    /* Let there be light...so we can see what we are doing */

    light1                                         = BrActorAdd(world, BrActorAllocate(BR_ACTOR_LIGHT, NULL));
    ((br_light *)light1->type_data)->type          = BR_LIGHT_DIRECT;
    ((br_light *)light1->type_data)->attenuation_c = BR_SCALAR(1.0);
    light1->t.type                                 = BR_TRANSFORM_MATRIX34;
    BrMatrix34RotateY(&light1->t.t.mat, BR_ANGLE_DEG(35));
    BrMatrix34RotateX(&light1->t.t.mat, BR_ANGLE_DEG(35));

    light2                                         = BrActorAdd(world, BrActorAllocate(BR_ACTOR_LIGHT, NULL));
    ((br_light *)light2->type_data)->type          = BR_LIGHT_DIRECT;
    ((br_light *)light2->type_data)->attenuation_c = BR_SCALAR(1.0);
    light2->t.type                                 = BR_TRANSFORM_MATRIX34;
    BrMatrix34RotateY(&light2->t.t.mat, BR_ANGLE_DEG(-35));
    BrMatrix34RotateX(&light2->t.t.mat, BR_ANGLE_DEG(-90));

    /* Mustn't forget to turn the light on */

    BrLightEnable(light1);
    BrLightEnable(light2);

    /* The environment map is attached to the earth-teapot pair so that */
    /* when rotated the reflection rotates accordingly */

    env         = BrActorAdd(pair, BrActorAllocate(BR_ACTOR_NONE, NULL));
    env->t.type = BR_TRANSFORM_MATRIX34;

    /* Rotate environment map so that the centre of it is in the same */
    /* direction as the earth */

    BrMatrix34RotateY(&env->t.t.mat, BR_ANGLE_DEG(-90));
    BrEnvironmentSet(env);

    float dt;
    while(UpdateSample(camera, &dt)) {
        /* Clear buffer and its Z buffer */

        BrPixelmapFill(colour_buffer, 0x0);
        BrPixelmapFill(depth_buffer, 0xffffffff);

        /* Copy plain environment map into mirror pixelmap. We will render */
        /* the earth over this. Also clear the Z buffer. */

        BrPixelmapCopy(mirror_pm, envmap1_pm);
        BrPixelmapFill(mirror_depth, 0xffffffff);

        /* When rendering the earth into the reflection we don't want to */
        /* see a view of the inside of the teapot. Turn it off for now */

        teapot->render_style = BR_RSTYLE_NONE;

        /* Render reflection and turn back on teapot */
        BrZbSceneRender(world, mirror_view, mirror_pm, mirror_depth);
        teapot->render_style = BR_RSTYLE_DEFAULT;

        /* Place rotated earth and moon */

        ++theta;

        BrMatrix34Scale(&earth->t.t.mat, BR_SCALAR(0.35), BR_SCALAR(0.35), BR_SCALAR(0.35));
        BrMatrix34PostRotateX(&earth->t.t.mat, BR_ANGLE_DEG(90));
        BrMatrix34PreRotateZ(&earth->t.t.mat, BR_ANGLE_DEG(3 * theta));

        BrMatrix34Scale(&moon->t.t.mat, BR_SCALAR(0.1), BR_SCALAR(0.1), BR_SCALAR(0.1));
        BrMatrix34PostTranslate(&moon->t.t.mat, BR_SCALAR(-0.7), BR_SCALAR(0), BR_SCALAR(0));
        BrMatrix34PostRotateY(&moon->t.t.mat, BR_ANGLE_DEG(-2 * theta));

        /* And now render the earth along with the shiny teapot */

        BrZbSceneRender(world, camera, colour_buffer, depth_buffer);
        BrPixelmapDoubleBuffer(screen, colour_buffer);
    }

    BrZbEnd();
    DOSGfxEnd();
    BrEnd();
}
