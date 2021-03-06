/*
 * copyright (c) 2016 Zhang Rui <bbcallen@gmail.com>
 *
 * This file is part of ijkPlayer.
 *
 * ijkPlayer is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * ijkPlayer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with ijkPlayer; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "internal.h"
#ifdef __APPLE__
#import <CoreVideo/CoreVideo.h>
#include "ijksdl_vout_overlay_videotoolbox.h"
#endif

static GLboolean yuv420sp_use(XM_GLES2_Renderer *renderer)
{
    ALOGI("use render yuv420sp\n");
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glUseProgram(renderer->program);            XM_GLES2_checkError_TRACE("glUseProgram");

    if (0 == renderer->plane_textures[0])
        glGenTextures(2, renderer->plane_textures);

    for (int i = 0; i < 2; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, renderer->plane_textures[i]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glUniform1i(renderer->us2_sampler[i], i);
    }

    glUniformMatrix3fv(renderer->um3_color_conversion, 1, GL_FALSE, XM_GLES2_getColorMatrix_bt709());

    return GL_TRUE;
}

static GLsizei yuv420sp_getBufferWidth(XM_GLES2_Renderer *renderer, SDL_VoutOverlay *overlay)
{
    if (!overlay)
        return 0;

    return overlay->pitches[0] / 1;
}

static GLboolean yuv420sp_uploadTexture(XM_GLES2_Renderer *renderer, SDL_VoutOverlay *overlay)
{
    if (!renderer || !overlay)
        return GL_FALSE;

    const GLsizei widths[2]    = { overlay->pitches[0], overlay->pitches[1] / 2 };
    const GLsizei heights[2]   = { overlay->h,          overlay->h / 2 };
    const GLubyte *pixels[2]   = { overlay->pixels[0],  overlay->pixels[1] };

    switch (overlay->format) {
        case SDL_FCC__VTB:
            break;
        default:
            ALOGE("[yuv420sp] unexpected format %x\n", overlay->format);
            return GL_FALSE;
    }

    glBindTexture(GL_TEXTURE_2D, renderer->plane_textures[0]);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RED_EXT,
                 widths[0],
                 heights[0],
                 0,
                 GL_RED_EXT,
                 GL_UNSIGNED_BYTE,
                 pixels[0]);

    glBindTexture(GL_TEXTURE_2D, renderer->plane_textures[1]);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RG_EXT,
                 widths[1],
                 heights[1],
                 0,
                 GL_RG_EXT,
                 GL_UNSIGNED_BYTE,
                 pixels[1]);

    return GL_TRUE;
}

XM_GLES2_Renderer *XM_GLES2_Renderer_create_yuv420sp(SDL_VoutOverlay *overlay)
{
    XM_Filter *filter = NULL;//XM_Filter_Factory_createTimeFilters(overlay);
    const char *fsh = XM_GLES2_getFragmentShader_yuv420sp();

    XM_GLES2_Renderer *renderer = XM_GLES2_Renderer_create_base(fsh, filter);
    if (!renderer)
        goto fail;

    renderer->us2_sampler[0] = glGetUniformLocation(renderer->program, "us2_SamplerX"); XM_GLES2_checkError_TRACE("glGetUniformLocation(us2_SamplerX)");
    renderer->us2_sampler[1] = glGetUniformLocation(renderer->program, "us2_SamplerY"); XM_GLES2_checkError_TRACE("glGetUniformLocation(us2_SamplerY)");

    renderer->um3_color_conversion = glGetUniformLocation(renderer->program, "um3_ColorConversion"); XM_GLES2_checkError_TRACE("glGetUniformLocation(um3_ColorConversionMatrix)");

    renderer->func_use            = yuv420sp_use;
    renderer->func_getBufferWidth = yuv420sp_getBufferWidth;
    renderer->func_uploadTexture  = yuv420sp_uploadTexture;

    return renderer;
fail:
    XM_GLES2_Renderer_free(renderer);
    return NULL;
}
