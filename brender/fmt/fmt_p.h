/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: fmt_p.h 1.1 1996/09/16 17:26:36 sam Exp $
 * $Locker: $
 *
 * Function prototypes for foreign file format support
 */
#ifndef _FMT_P_H_
#define _FMT_P_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 3D Studio .3DS files
 */
br_fmt_results * BR_PUBLIC_ENTRY BrFmt3DSLoad(const char *name, br_fmt_options *fmt_options, br_3ds_options *options);

/*
 * 3D Studio .ASC files
 */
br_uint_32 BR_PUBLIC_ENTRY BrFmtASCLoad(const char *name, br_model **mtable, br_uint_16 max_models);

/*
 * Eric Haines' NFF format
 */
br_model * BR_PUBLIC_ENTRY BrFmtNFFLoad(const char *name);

/*
 * Script files
 */
br_uint_32 BR_PUBLIC_ENTRY BrFmtScriptMaterialLoadMany(const char *filename, br_material **materials, br_uint_16 num);
br_material * BR_PUBLIC_ENTRY BrFmtScriptMaterialLoad(const char *filename);

br_uint_32 BR_PUBLIC_ENTRY BrFmtScriptMaterialSaveMany(const char *filename, br_material **materials, br_uint_16 num);
br_uint_32 BR_PUBLIC_ENTRY BrFmtScriptMaterialSave(const char *filename, br_material *ptr);

/*
 * Windows .BMP files
 */
br_pixelmap * BR_PUBLIC_ENTRY BrFmtBMPLoad(const char *name,br_uint_32 flags);

/*
 * .TGA files
 */
br_pixelmap * BR_PUBLIC_ENTRY BrFmtTGALoad(const char *name,br_uint_32 flags);

/*
 * .GIF files
 */
br_pixelmap * BR_PUBLIC_ENTRY BrFmtGIFLoad(const char *name,br_uint_32 flags);

/*
 * .IFF/.LBM files
 */
br_pixelmap * BR_PUBLIC_ENTRY BrFmtIFFLoad(const char *name,br_uint_32 flags);

#ifdef __cplusplus
};
#endif
#endif
