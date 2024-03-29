#include "drv.h"
#include <brassert.h>

void *DevicePixelmapGLExtCreateContext(br_device_pixelmap *self)
{
    UASSERT(self->use_type == BRT_NONE);
    return self->asFront.ext_procs.create_context((br_pixelmap *)self, self->asFront.ext_procs.user);
}

void DevicePixelmapGLExtDeleteContext(br_device_pixelmap *self, void *ctx)
{
    UASSERT(self->use_type == BRT_NONE);
    self->asFront.ext_procs.delete_context((br_pixelmap *)self, ctx, self->asFront.ext_procs.user);
}

br_error DevicePixelmapGLExtMakeCurrent(br_device_pixelmap *self, void *ctx)
{
    UASSERT(self->use_type == BRT_NONE);
    return self->asFront.ext_procs.make_current((br_pixelmap *)self, ctx, self->asFront.ext_procs.user);
}

void DevicePixelmapGLExtSwapBuffers(br_device_pixelmap *self)
{
    UASSERT(self->use_type == BRT_NONE);
    self->asFront.ext_procs.swap_buffers((br_pixelmap *)self, self->asFront.ext_procs.user);
}

br_error DevicePixelmapGLExtResize(br_device_pixelmap *self, br_int_32 w, br_int_32 h)
{
    UASSERT(self->use_type == BRT_NONE);
    if(self->asFront.ext_procs.resize == NULL)
        return BRE_OK;

    return self->asFront.ext_procs.resize((br_pixelmap *)self, w, h, self->asFront.ext_procs.user);
}

br_device_pixelmap_gl_getprocaddress_cbfn *DevicePixelmapGLExtGetGetProcAddress(br_device_pixelmap *self)
{
    UASSERT(self->use_type == BRT_NONE);
    return self->asFront.ext_procs.get_proc_address;
}

void DevicePixelmapGLExtPreSwap(br_device_pixelmap *self, GLuint fbo)
{
    UASSERT(self->use_type == BRT_NONE);
    if(self->asFront.ext_procs.preswap_hook != NULL)
        self->asFront.ext_procs.preswap_hook((br_pixelmap *)self, fbo, self->asFront.ext_procs.user);
}

void DevicePixelmapGLExtFree(br_device_pixelmap *self)
{
    UASSERT(self->use_type == BRT_NONE);
    if(self->asFront.ext_procs.free != NULL)
        self->asFront.ext_procs.free((br_pixelmap *)self, self->asFront.ext_procs.user);
}

br_error DevicePixelmapGLExtHandleWindowEvent(br_device_pixelmap *self, void *arg)
{
    UASSERT(self->use_type == BRT_NONE);
    if(self->asFront.ext_procs.handle_window_event == NULL)
        return BRE_OK;

    return self->asFront.ext_procs.handle_window_event((br_pixelmap *)self, arg, self->asFront.ext_procs.user);
}
