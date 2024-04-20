/**
 * @file Plugin.h
 * @date Sat, 20th April 2024
 * @author Siddharth Mishra (admin@brightprogrammer.in)
 * @copyright Copyright 2024 Siddharth Mishra
 * @copyright Copyright 2024 Anvie Labs
 *
 * Copyright 2024 Siddharth Mishra, Anvie Labs
 * 
 * Redistribution and use in source and binary forms, with or without modification, are permitted 
 * provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions
 *    and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions
 *    and the following disclaimer in the documentation and/or other materials provided with the
 *    distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse
 *    or promote products derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * */

#ifndef ANVIE_CROSSGUI_PLUGIN_PLUGIN_H
#define ANVIE_CROSSGUI_PLUGIN_PLUGIN_H

#include <Anvie/Types.h>

/**
 * @b Generic data needs to be type-casted to plugin-specific data,
 * depending on @c type of plugin in @x XuiPlugin object.
 * */
typedef void *XuiPluginGenericData;

typedef Bool (*XuiPluginInit) (XuiPluginGenericData plugin_data);
typedef Bool (*XuiPluginDeinit) (XuiPluginGenericData plugin_data);

typedef struct XuiPluginVersion {
    Uint16 year;
    Uint8  month;
    Uint8  date;
} XuiPluginVersion;

typedef enum XuiPluginType {
    XUI_PLUGIN_TYPE_NONE     = 0,
    XUI_PLUGIN_TYPE_GRAPHICS = 1, /**< @b Defined in Plugin/Graphics */
    XUI_PLUGIN_TYPE_MAX
} XuiPluginType;

typedef enum XuiPluginPlatformMask {
    XUI_PLUGIN_PLATFORM_MASK_NONE    = 0, /**< @b No platform supported. (Must not be the case) */
    XUI_PLUGIN_PLATFORM_MASK_LINUX   = (1 << 0), /**< @b Plugin can work on Linux. */
    XUI_PLUGIN_PLATFORM_MASK_WINDOWS = (1 << 1), /**< @b Plugin can work on windows. */
    XUI_PLUGIN_PLATFORM_MASK_MAC     = (1 << 2), /**< @b Plugin can work on MacOS */
    XUI_PLUGIN_PLATFORM_MASK_ANDROID = (1 << 3), /**< @b Plugin can work on android */
    XUI_PLUGIN_PLATFORM_MASK_ALL     = -1        /**< @b The plugin works on all platforms */
} XuiPluginPlatfomMask;

typedef struct XuiPlugin {
    XuiPluginType        type;        /**< @b Type of plugin */
    CString              name;        /**< @b Name of plugin. */
    XuiPluginVersion     version;     /**< @b Plugin version. */
    CString              license;     /**< @b License information about plugin.*/
    XuiPluginGenericData plugin_data; /**< @b Data of plugin dependent on plugin type. */
    XuiPluginInit        init;        /**< @b Initialize plugin. */
    XuiPluginDeinit      deinit;      /**< @b De-initialize plugin. */
} XuiPlugin;

#endif                                // ANVIE_CROSSGUI_PLUGIN_PLUGIN_H
