/*
* Created by sunyc on 2018-6-25.
* Create Fragment shaders of blackcat effects
*/

#include "ijksdl/gles2_xm/internal.h"

static const char g_shader_blackcat[] = XM_GLES_STRING(
    precision highp float;
    varying   highp vec2 vv2_Texcoord;
    uniform   lowp  sampler2D us2_Sampler;
    uniform   lowp  sampler2D curve; // lookup texture

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{
    float GreyVal;
    lowp vec4 textureColor;
    lowp vec4 textureColorOri;
    float xCoordinate = vv2_Texcoord.x;
    float yCoordinate = vv2_Texcoord.y;

    highp float redCurveValue;
    highp float greenCurveValue;
    highp float blueCurveValue;
    textureColor = texture2D( us2_Sampler, vec2(xCoordinate, yCoordinate));
    // step1 curve
    redCurveValue = texture2D(curve, vec2(textureColor.r, 0.0)).r;
    greenCurveValue = texture2D(curve, vec2(textureColor.g, 0.0)).g;
    blueCurveValue = texture2D(curve, vec2(textureColor.b, 0.0)).b;

    //textureColor = vec4(redCurveValue, greenCurveValue, blueCurveValue, 1.0);
    vec3 tColor = vec3(redCurveValue, greenCurveValue, blueCurveValue);
    tColor = rgb2hsv(tColor);

    tColor.g = tColor.g * 1.2;

    float dStrength = 1.0;
    float dSatStrength = 0.3;

    float dGap = 0.0;

    if( tColor.r >= 0.0 && tColor.r < 0.417)
    {
        tColor.g = tColor.g + (tColor.g * dSatStrength);
    }
    else if( tColor.r > 0.958 && tColor.r <= 1.0)
    {
        tColor.g = tColor.g + (tColor.g * dSatStrength);
    }
    else if( tColor.r >= 0.875 && tColor.r <= 0.958)
    {
        dGap = abs(tColor.r - 0.875);
        dStrength = (dGap / 0.0833);

        tColor.g = tColor.g + (tColor.g * dSatStrength * dStrength);
    }
    else if( tColor.r >= 0.0417 && tColor.r <= 0.125)
    {
        dGap = abs(tColor.r - 0.125);
        dStrength = (dGap / 0.0833);

        tColor.g = tColor.g + (tColor.g * dSatStrength * dStrength);
    }

    tColor = hsv2rgb(tColor);
    tColor = clamp(tColor, 0.0, 1.0);

    redCurveValue = texture2D(curve, vec2(tColor.r, 1.0)).r;
    greenCurveValue = texture2D(curve, vec2(tColor.g, 1.0)).r;
    blueCurveValue = texture2D(curve, vec2(tColor.b, 1.0)).r;

    redCurveValue = texture2D(curve, vec2(redCurveValue, 1.0)).g;
    greenCurveValue = texture2D(curve, vec2(greenCurveValue, 1.0)).g;
    blueCurveValue = texture2D(curve, vec2(blueCurveValue, 1.0)).g;

    textureColor = vec4(redCurveValue, greenCurveValue, blueCurveValue, 1.0);

    gl_FragColor = vec4(textureColor.r, textureColor.g, textureColor.b, 1.0);
}
);

const char *XM_BFilter_getFragmentShader_blackcat()
{
    return g_shader_blackcat;
}

