
#include "brender.h"
#include "brddi.h"
#include "mcga.h"

/*
 * hack
 */
br_device *BR_EXPORT Drv1SDL2Begin(const char *identifier, const char *arguments);

/*
 * Main entry point for device - this may get redefined by the build system.
 */
br_device *BR_EXPORT BrDrv1MCGABegin(const char *arguments)
{
    return Drv1SDL2Begin("MCGA", arguments);
}

#ifdef DEFINE_BR_ENTRY_POINT
br_device *BR_EXPORT BrDrv1Begin(const char *arguments)
{
    return BrDrv1MCGABegin(arguments);
}
#endif
