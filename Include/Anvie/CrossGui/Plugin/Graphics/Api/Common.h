/**
 * @file Common.h
 * @date Wed, 15th May 2024
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

#ifndef ANVIE_CROSSGUI_PLUGIN_GRAPHICS_API_COMMON_H
#define ANVIE_CROSSGUI_PLUGIN_GRAPHICS_API_COMMON_H

/**
 * @b Set of possible values returned by render/draw methods.
 * */
typedef enum XuiRenderStatus {
    /**
     * @b Treated same as @x XUI_RENDER_STATUS_ERR
     * */
    XUI_RENDER_STATUS_UNKNOWN = 0,

    /**
     * @b Something bad happened, and it's not recoverable. Cannot continue.
     * 
     * When a render (draw) method returns this value, the plugin must not call
     * the draw method again because that won't solve the problem. The method to
     * solve this is probably plugin dependent.
     * */
    XUI_RENDER_STATUS_ERR = XUI_RENDER_STATUS_UNKNOWN,

    /**
    * @b Everything's ok! keep going...
    *
    * This return value from a render (draw) method means the draw
    * call was processed successfully and user code can continue to
    * render.
    * */
    XUI_RENDER_STATUS_OK,

    /**
     * @b Something bad happened, but continue to recover from it. 
     * 
     * When a render (draw) method returns this value, the plugin user must
     * not continue further but treat that frame as dropped, and try to render again
     * from the beginning. This will automatically recover the renderer from the error.
     * 
     * This usually happens when the window is resized or is moved from one screen
     * to other.
     * */
    XUI_RENDER_STATUS_CONTINUE,

    XUI_RENDER_STATUS_MAX /**< @b Number of render status enums */
} XuiRenderStatus;

#endif                    // ANVIE_CROSSGUI_PLUGIN_GRAPHICS_API_COMMON_H
