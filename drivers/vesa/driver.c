
#include "brender.h"
#include "brddi.h"
#include "vesa.h"

/*
 * hack
 */
br_device *BR_EXPORT Drv1SDL2Begin(const char *identifier, const char *arguments);

/*
 * Main entry point for device - this may get redefined by the build system.
 */
br_device *BR_EXPORT BrDrv1VESABegin(const char *arguments)
{
    return Drv1SDL2Begin("VESA", arguments);
}

#ifdef DEFINE_BR_ENTRY_POINT
br_device *BR_EXPORT BrDrv1Begin(const char *arguments)
{
    return BrDrv1VESABegin(arguments);
}
#endif
