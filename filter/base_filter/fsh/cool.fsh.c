/*
* Created by sunyc on 2018-6-25.
* Create Fragment shaders of cool effects
*/

#include "ijksdl/gles2_xm/internal.h"

static const char g_shader_cool[] = XM_GLES_STRING(
precision highp float;
varying highp vec2 vv2_Texcoord;
uniform lowp sampler2D us2_Sampler;

uniform lowp sampler2D curve;

void main()
{
    lowp vec4 textureColor;
    lowp vec4 textureColorOri;

    float xCoordinate = vv2_Texcoord.x;
    float yCoordinate = vv2_Texcoord.y;

    highp float redCurveValue;
    highp float greenCurveValue;
    highp float blueCurveValue;

    textureColor = texture2D( us2_Sampler, vec2(xCoordinate, yCoordinate));
    textureColorOri = textureColor;
    // step1 curve
    redCurveValue = texture2D(curve, vec2(textureColor.r, 0.0)).r;
    greenCurveValue = texture2D(curve, vec2(textureColor.g, 0.0)).g;
    blueCurveValue = texture2D(curve, vec2(textureColor.b, 0.0)).b;
    // step2 level
    redCurveValue = texture2D(curve, vec2(redCurveValue, 0.0)).a;
    greenCurveValue = texture2D(curve, vec2(greenCurveValue, 0.0)).a;
    blueCurveValue = texture2D(curve, vec2(blueCurveValue, 0.0)).a;
    // step3 brightness/constrast adjust
    redCurveValue = redCurveValue * 1.25 - 0.12549;
    greenCurveValue = greenCurveValue * 1.25 - 0.12549;
    blueCurveValue = blueCurveValue * 1.25 - 0.12549;
    //redCurveValue = (((redCurveValue) > (1.0)) ? (1.0) : (((redCurveValue) < (0.0)) ? (0.0) : (redCurveValue)));
    //greenCurveValue = (((greenCurveValue) > (1.0)) ? (1.0) : (((greenCurveValue) < (0.0)) ? (0.0) : (greenCurveValue)));
    //blueCurveValue = (((blueCurveValue) > (1.0)) ? (1.0) : (((blueCurveValue) < (0.0)) ? (0.0) : (blueCurveValue)));
    // step4 normal blending with original
    textureColor = vec4(redCurveValue, greenCurveValue, blueCurveValue, 1.0);
    textureColor = (textureColorOri - textureColor) * 0.549 + textureColor;

    gl_FragColor = vec4(textureColor.r, textureColor.g, textureColor.b, 1.0);
}
);

const char *XM_BFilter_getFragmentShader_cool()
{
    return g_shader_cool;
}
