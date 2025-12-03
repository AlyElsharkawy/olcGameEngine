#pragma once
#include "olcPixelGameEngine.h"
#include "miscPrimitives.h"


//Adapted from olcPixelGameEngine.h to use depthBuffer
void DrawTriangleWithDepthBuffer(const Triangle& triangleInput, const RenderingInstance& RI, olc::Pixel p = olc::WHITE);
void DrawTriangleWithDepthBufferInline(int32_t x1, int32_t y1, float z1,
                                 int32_t x2, int32_t y2, float z2,
                                 int32_t x3, int32_t y3, float z3,
                                 const RenderingInstance& RI,
                                 olc::Pixel p);

void FillTriangleWithDepthBuffer(const Triangle& triangleInput, const RenderingInstance& RI, olc::Pixel p = olc::WHITE);
void FillTriangleWithDepthBufferInline(int32_t x1, int32_t y1, float w1,
                                 int32_t x2, int32_t y2, float w2,
                                 int32_t x3, int32_t y3, float w3,
                                 const RenderingInstance& RI,
                                 olc::Pixel p);

void DrawLineWithDepthBufferInline(int32_t x1, int32_t y1, float w1,
                             int32_t x2, int32_t y2, float w2,
                             const RenderingInstance& RI,
                             olc::Pixel p = olc::WHITE, uint32_t pattern = 0xFFFFFFFF);

void DrawTexturedTriangle(const RenderingInstance& RI, const Triangle& input,
                          const olc::Pixel& pixelIllumunation, const olc::Sprite* texture);

