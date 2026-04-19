// 文件: transparent_png.h
// 功能: 为 EasyX 提供 PNG 透明贴图支持(无需掩码图)

#ifndef TRANSPARENT_PNG_H
#define TRANSPARENT_PNG_H

#include <graphics.h>

/**
 * 绘制带透明通道的 PNG 图片(完整绘制)
 * @param picture   图片指针
 * @param x         目标 X 坐标
 * @param y         目标 Y 坐标
 */
inline void putimagePNG(IMAGE* picture, int x, int y) {
    if (!picture) return;
    int picW = picture->getwidth();
    int picH = picture->getheight();
    int winW = getwidth();
    int winH = getheight();

    DWORD* dst = GetImageBuffer();           // 窗口显存
    DWORD* src = GetImageBuffer(picture);    // 图片显存

    for (int iy = 0; iy < picH; iy++) {
        for (int ix = 0; ix < picW; ix++) {
            int dstX = ix + x;
            int dstY = iy + y;

            // 边界检查
            if (dstX < 0 || dstX >= winW || dstY < 0 || dstY >= winH)
                continue;

            int srcIdx = ix + iy * picW;
            int dstIdx = dstX + dstY * winW;

            // 提取源像素的 ARGB 分量
            int sa = (src[srcIdx] & 0xFF000000) >> 24;  // Alpha 通道
            int sr = (src[srcIdx] & 0xFF0000) >> 16;    // Red
            int sg = (src[srcIdx] & 0xFF00) >> 8;       // Green
            int sb = src[srcIdx] & 0xFF;                // Blue

            if (sa == 0) continue;  // 完全透明, 不绘制
            if (sa == 255) {
                // 完全不透明, 直接覆盖
                dst[dstIdx] = src[srcIdx];
                continue;
            }

            // 提取目标像素分量
            int dr = (dst[dstIdx] & 0xFF0000) >> 16;
            int dg = (dst[dstIdx] & 0xFF00) >> 8;
            int db = dst[dstIdx] & 0xFF;

            // Alpha 混合公式: 结果 = 源 * alpha/255 + 背景 * (1 - alpha/255)
            int rr = (sr * sa + dr * (255 - sa)) / 255;
            int rg = (sg * sa + dg * (255 - sa)) / 255;
            int rb = (sb * sa + db * (255 - sa)) / 255;

            dst[dstIdx] = (rr << 16) | (rg << 8) | rb;
        }
    }
}

/**
 * 绘制带透明通道的 PNG 图片(支持裁剪区域与透明度调节)
 * @param picture   图片指针
 * @param dstX      目标 X 坐标
 * @param dstY      目标 Y 坐标
 * @param srcX      图片裁剪起始 X
 * @param srcY      图片裁剪起始 Y
 * @param width     绘制宽度
 * @param height    绘制高度
 * @param alpha     整体透明度 (0.0 ~ 1.0), 默认 1.0 完全不透明
 */
inline void putimagePNG(IMAGE* picture, int dstX, int dstY,
    int srcX, int srcY, int width, int height,
    double alpha = 1.0) {
    if (!picture) return;

    int picW = picture->getwidth();
    int picH = picture->getheight();
    int winW = getwidth();
    int winH = getheight();

    // 裁剪区域越界保护
    if (srcX < 0) srcX = 0;
    if (srcY < 0) srcY = 0;
    if (srcX + width > picW) width = picW - srcX;
    if (srcY + height > picH) height = picH - srcY;
    if (width <= 0 || height <= 0) return;

    DWORD* dst = GetImageBuffer();
    DWORD* src = GetImageBuffer(picture);

    for (int iy = 0; iy < height; iy++) {
        for (int ix = 0; ix < width; ix++) {
            int curDstX = dstX + ix;
            int curDstY = dstY + iy;

            if (curDstX < 0 || curDstX >= winW || curDstY < 0 || curDstY >= winH)
                continue;

            int srcIdx = (srcX + ix) + (srcY + iy) * picW;
            int dstIdx = curDstX + curDstY * winW;

            // ARGB 提取
            int sa = ((src[srcIdx] & 0xFF000000) >> 24) * alpha;
            if (sa <= 0) continue;

            int sr = (src[srcIdx] & 0xFF0000) >> 16;
            int sg = (src[srcIdx] & 0xFF00) >> 8;
            int sb = src[srcIdx] & 0xFF;

            if (sa >= 255) {
                dst[dstIdx] = src[srcIdx];
                continue;
            }

            int dr = (dst[dstIdx] & 0xFF0000) >> 16;
            int dg = (dst[dstIdx] & 0xFF00) >> 8;
            int db = dst[dstIdx] & 0xFF;

            int rr = (sr * sa + dr * (255 - sa)) / 255;
            int rg = (sg * sa + dg * (255 - sa)) / 255;
            int rb = (sb * sa + db * (255 - sa)) / 255;

            dst[dstIdx] = (rr << 16) | (rg << 8) | rb;
        }
    }
}

#endif // TRANSPARENT_PNG_H#pragma once
