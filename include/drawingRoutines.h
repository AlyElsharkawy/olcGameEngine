#pragma once
#include "olcPixelGameEngine.h"
#include "miscPrimitives.h"

//Adapted from olcPixelGameEngine.h to use depthBuffer
void DrawTriangleWithDepthBuffer(const Triangle& triangleInput, const RenderingInstance& RI, olc::Pixel p = olc::WHITE);
void DrawTriangleWithDepthBuffer(int32_t x1, int32_t y1, int32_t z1,
                                 int32_t x2, int32_t y2, int32_t z2,
                                 int32_t x3, int32_t y3, int32_t z3,
                                 const RenderingInstance& RI,
                                 olc::Pixel p);

void FillTriangleWithDepthBuffer(const Triangle& triangleInput, const RenderingInstance& RI, olc::Pixel p = olc::WHITE);
void FillTriangleWithDepthBuffer(int32_t x1, int32_t y1, int32_t z1,
                                 int32_t x2, int32_t y2, int32_t z2,
                                 int32_t x3, int32_t y3, int32_t z3,
                                 const RenderingInstance& RI,
                                 olc::Pixel p);

void DrawLineWithDepthBuffer(int32_t x1, int32_t y1, int32_t z1,
                             int32_t x2, int32_t y2, int32_t z2,
                             const RenderingInstance& RI,
                             olc::Pixel p = olc::WHITE, uint32_t pattern = 0xFFFFFFFF);

void DrawTexturedTriangle(const RenderingInstance& RI, const Triangle& input, const olc::Sprite* texture);
