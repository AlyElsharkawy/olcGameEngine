#include "drawingRoutines.h"
#include "essentialFunctions.h"
#include "miscFunctions.h"
#include "miscPrimitives.h"

void DrawTexturedTriangle(const RenderingInstance &RI, const Triangle &input,
                          const NormalizedPixel& pixelIllumination, const olc::Sprite *texture) {
  const int screenSize = RI.engine->ScreenHeight() * RI.engine->ScreenWidth();
  int x1 = input.points[0].x;
  int x2 = input.points[1].x;
  int x3 = input.points[2].x;

  int y1 = input.points[0].y;
  int y2 = input.points[1].y;
  int y3 = input.points[2].y;

  float u1 = input.texels[0].u;
  float u2 = input.texels[1].u;
  float u3 = input.texels[2].u;

  float v1 = input.texels[0].v;
  float v2 = input.texels[1].v;
  float v3 = input.texels[2].v;

  float w1 = input.texels[0].w;
  float w2 = input.texels[1].w;
  float w3 = input.texels[2].w;

  // if(x1 < 0 || y1 < 0 || x2 < 0 || y2 < 0 || x3 < 0 || y3 < 0 || w1 < 0 || w2
  // < 0 || w3 < 0) return;

  if (y2 < y1) {
    swap(y1, y2);
    swap(x1, x2);
    swap(u1, u2);
    swap(v1, v2);
    swap(w1, w2);
  }

  if (y3 < y1) {
    swap(y1, y3);
    swap(x1, x3);
    swap(u1, u3);
    swap(v1, v3);
    swap(w1, w3);
  }

  if (y3 < y2) {
    swap(y2, y3);
    swap(x2, x3);
    swap(u2, u3);
    swap(v2, v3);
    swap(w2, w3);
  }

  int dy1 = y2 - y1;
  int dx1 = x2 - x1;
  float dv1 = v2 - v1;
  float du1 = u2 - u1;
  float dw1 = w2 - w1;

  int dy2 = y3 - y1;
  int dx2 = x3 - x1;
  float dv2 = v3 - v1;
  float du2 = u3 - u1;
  float dw2 = w3 - w1;

  float tex_u, tex_v, tex_w;

  float dax_step = 0, dbx_step = 0, du1_step = 0, dv1_step = 0, du2_step = 0,
        dv2_step = 0, dw1_step = 0, dw2_step = 0;

  if (dy1)
    dax_step = dx1 / (float)abs(dy1);
  if (dy2)
    dbx_step = dx2 / (float)abs(dy2);

  if (dy1)
    du1_step = du1 / (float)abs(dy1);
  if (dy1)
    dv1_step = dv1 / (float)abs(dy1);
  if (dy1)
    dw1_step = dw1 / (float)abs(dy1);

  if (dy2)
    du2_step = du2 / (float)abs(dy2);
  if (dy2)
    dv2_step = dv2 / (float)abs(dy2);
  if (dy2)
    dw2_step = dw2 / (float)abs(dy2);

  if (dy1) {
    for (int i = y1; i <= y2; i++) {
      int ax = x1 + (float)(i - y1) * dax_step;
      int bx = x1 + (float)(i - y1) * dbx_step;

      float tex_su = u1 + (float)(i - y1) * du1_step;
      float tex_sv = v1 + (float)(i - y1) * dv1_step;
      float tex_sw = w1 + (float)(i - y1) * dw1_step;

      float tex_eu = u1 + (float)(i - y1) * du2_step;
      float tex_ev = v1 + (float)(i - y1) * dv2_step;
      float tex_ew = w1 + (float)(i - y1) * dw2_step;

      if (ax > bx) {
        swap(ax, bx);
        swap(tex_su, tex_eu);
        swap(tex_sv, tex_ev);
        swap(tex_sw, tex_ew);
      }

      tex_u = tex_su;
      tex_v = tex_sv;
      tex_w = tex_sw;

      float tstep = 1.0f / ((float)(bx - ax));
      float t = 0.0f;

      for (int j = ax; j < bx; j++) {
        tex_u = (1.0f - t) * tex_su + t * tex_eu;
        tex_v = (1.0f - t) * tex_sv + t * tex_ev;
        tex_w = (1.0f - t) * tex_sw + t * tex_ew;
        // J is X and I is Y
        int offset = i * RI.engine->ScreenWidth() + j;
        if (offset < screenSize && tex_w > RI.depthBuffer[offset]) {
          NormalizedPixel tempPixel = SampleNormalizedPixel(texture, (tex_u / tex_w), (tex_v / tex_w));
          MultiplyNormalizedPixelPixel(tempPixel, pixelIllumination);
          olc::Pixel finalPixel = ClampPixel(tempPixel);
          //olc::Pixel finalPixel = texture->Sample(tex_u / tex_w, tex_v / tex_w);
          RI.engine->Draw(j, i, finalPixel);
          RI.depthBuffer[i * RI.engine->ScreenWidth() + j] = tex_w;
        }
        t += tstep;
      }
    }
  }

  dy1 = y3 - y2;
  dx1 = x3 - x2;
  dv1 = v3 - v2;
  du1 = u3 - u2;
  dw1 = w3 - w2;

  if (dy1)
    dax_step = dx1 / (float)abs(dy1);
  if (dy2)
    dbx_step = dx2 / (float)abs(dy2);

  du1_step = 0, dv1_step = 0;
  if (dy1)
    du1_step = du1 / (float)abs(dy1);
  if (dy1)
    dv1_step = dv1 / (float)abs(dy1);
  if (dy1)
    dw1_step = dw1 / (float)abs(dy1);

  if (dy1) {
    for (int i = y2; i <= y3; i++) {
      int ax = x2 + (float)(i - y2) * dax_step;
      int bx = x1 + (float)(i - y1) * dbx_step;

      float tex_su = u2 + (float)(i - y2) * du1_step;
      float tex_sv = v2 + (float)(i - y2) * dv1_step;
      float tex_sw = w2 + (float)(i - y2) * dw1_step;

      float tex_eu = u1 + (float)(i - y1) * du2_step;
      float tex_ev = v1 + (float)(i - y1) * dv2_step;
      float tex_ew = w1 + (float)(i - y1) * dw2_step;

      if (ax > bx) {
        swap(ax, bx);
        swap(tex_su, tex_eu);
        swap(tex_sv, tex_ev);
        swap(tex_sw, tex_ew);
      }

      tex_u = tex_su;
      tex_v = tex_sv;
      tex_w = tex_sw;

      float tstep = 1.0f / ((float)(bx - ax));
      float t = 0.0f;

      for (int j = ax; j < bx; j++) {
        tex_u = (1.0f - t) * tex_su + t * tex_eu;
        tex_v = (1.0f - t) * tex_sv + t * tex_ev;
        tex_w = (1.0f - t) * tex_sw + t * tex_ew;

        int offset = i * RI.engine->ScreenWidth() + j;
        if (offset < screenSize && tex_w > RI.depthBuffer[offset]) {
          NormalizedPixel tempPixel = SampleNormalizedPixel(texture, (tex_u / tex_w), (tex_v / tex_w));
          MultiplyNormalizedPixelPixel(tempPixel, pixelIllumination);
          olc::Pixel finalPixel = ClampPixel(tempPixel);
          //olc::Pixel finalPixel = texture->Sample(tex_u / tex_w, tex_v / tex_w);
          // J is X and I is Y
          RI.engine->Draw(j, i, finalPixel);
          RI.depthBuffer[offset] = tex_w;
        }
        t += tstep;
      }
    }
  }
}

void DrawTriangleWithDepthBuffer(const Triangle &triangleInput,
                                 const RenderingInstance &RI, olc::Pixel p) {
  DrawTriangleWithDepthBufferInline(
      triangleInput.points[0].x, triangleInput.points[0].y,
      triangleInput.points[0].z, triangleInput.points[1].x,
      triangleInput.points[1].y, triangleInput.points[1].z,
      triangleInput.points[2].x, triangleInput.points[2].y,
      triangleInput.points[2].z, RI, p);
}

// WARNING: This does not perfectly interpolate the W value. This bug will NOT
// be fixed in the foreseeable future
void DrawLineWithDepthBufferInline(int32_t x1, int32_t y1, float w1, int32_t x2,
                                   int32_t y2, float w2,
                                   const RenderingInstance &RI, olc::Pixel p,
                                   uint32_t pattern) 
{
int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
		dx = x2 - x1; dy = y2 - y1;

		auto rol = [&](void) { pattern = (pattern << 1) | (pattern >> 31); return pattern & 1; };
		olc::vi2d p1(x1, y1), p2(x2, y2);
    float wVal = std::min(w1, w2);
		if (!RI.engine->ClipLineToScreen(p1, p2))
			return;
		x1 = p1.x; y1 = p1.y;
		x2 = p2.x; y2 = p2.y;

		// straight lines idea by gurkanctn
		if (dx == 0) // Line is vertical
		{
			if (y2 < y1) std::swap(y1, y2);
      int index;
			for (y = y1; y <= y2; y++) 
      {
        // J is X and I is Y
        index = y * RI.engine->ScreenWidth() + x1;
        if( wVal > RI.depthBuffer[index] && rol())
        {
          RI.engine->Draw(x1, y, p);
          RI.depthBuffer[index] = w1;
        } 
      }
			return;
		}

		if (dy == 0) // Line is horizontal
		{
			if (x2 < x1) std::swap(x1, x2);
      int index;
			for (x = x1; x <= x2; x++)
      { 
        // J is X and I is Y
        index = y1 * RI.engine->ScreenWidth() + x;
        if (wVal > RI.depthBuffer[index] && rol()) 
        {
          RI.engine->Draw(x, y1, p);
          RI.depthBuffer[index] = w2;
        }
      }
			return;
		}

		// Line is Funk-aye
		dx1 = abs(dx); dy1 = abs(dy);
		px = 2 * dy1 - dx1;	py = 2 * dx1 - dy1;
		if (dy1 <= dx1)
		{
			if (dx >= 0)
			{
				x = x1; y = y1; xe = x2;
			}
			else
			{
				x = x2; y = y2; xe = x1;
			}
      // J is X and I is Y
      int tempIndex = y * RI.engine->ScreenWidth() + x;
			if (wVal > RI.depthBuffer[tempIndex] && rol()) 
      {
        RI.engine->Draw(x, y, p);
        RI.depthBuffer[tempIndex] = wVal;
      }

			for (i = 0; x < xe; i++)
			{
				x = x + 1;
				if (px < 0)
					px = px + 2 * dy1;
				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) y = y + 1; else y = y - 1;
					px = px + 2 * (dy1 - dx1);
				}
        // J is X and I is Y
        int tempIndex = y * RI.engine->ScreenWidth() + x;
				if(rol())
        {
          RI.engine->Draw(x, y, p);
          RI.depthBuffer[tempIndex] = wVal;
        }
			}
		}
		else
		{
			if (dy >= 0)
			{
				x = x1; y = y1; ye = y2;
			}
			else
			{
				x = x2; y = y2; ye = y1;
			}

      // J is X and I is Y
      int tempIndex = y * RI.engine->ScreenWidth() + x;
			if(wVal > RI.depthBuffer[tempIndex] && rol())
      {
        RI.engine->Draw(x, y, p);
        RI.depthBuffer[tempIndex] = wVal;
      }

			for (i = 0; y < ye; i++)
			{
				y = y + 1;
				if (py <= 0)
					py = py + 2 * dx1;
				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) x = x + 1; else x = x - 1;
					py = py + 2 * (dx1 - dy1);
				}
        // J is X and I is Y
        int tempIndex = y * RI.engine->ScreenWidth() + x;
				if(wVal > RI.depthBuffer[tempIndex] &&  rol()) 
        {
          RI.engine->Draw(x, y, p);
          RI.depthBuffer[tempIndex] = wVal;
        }
			}
		}
}

void DrawTriangleWithDepthBufferInline(int32_t x1, int32_t y1, float w1,
                                       int32_t x2, int32_t y2, float w2,
                                       int32_t x3, int32_t y3, float w3,
                                       const RenderingInstance &RI,
                                       olc::Pixel p) {
  DrawLineWithDepthBufferInline(x1, y1, 1.0f / w1, x2, y2, 1.0f / w2, RI);
  DrawLineWithDepthBufferInline(x2, y2, 1.0f / w2, x3, y3, 1.0f / w3, RI);
  DrawLineWithDepthBufferInline(x1, y1, 1.0f / w1, x3, y3, 1.0f / w3, RI);
}

void FillTriangleWithDepthBuffer(const Triangle &triangleInput,
                                 const RenderingInstance &RI, olc::Pixel p) {
  FillTriangleWithDepthBufferInline(
      triangleInput.points[0].x, triangleInput.points[0].y, 1.0f / triangleInput.points[0].w, 
      triangleInput.points[1].x, triangleInput.points[1].y, 1.0f / triangleInput.points[1].w,
      triangleInput.points[2].x, triangleInput.points[2].y, 1.0f / triangleInput.points[2].w, RI, p);
}

void FillTriangleWithDepthBufferInline(int32_t x1, int32_t y1, float w1,
                                       int32_t x2, int32_t y2, float w2,
                                       int32_t x3, int32_t y3, float w3,
                                       const RenderingInstance &RI,
                                       olc::Pixel p) {
  // if(x1 < 0 || y1 < 0 || x2 < 0 || y2 < 0 || x3 < 0 || y3 < 0 || w1 < 0 || w2
  // < 0 || w3 < 0) return;
  const int screenSize = RI.engine->ScreenHeight() * RI.engine->ScreenWidth();
  if (y2 < y1) {
    swap(y1, y2);
    swap(x1, x2);
    swap(w1, w2);
  }

  if (y3 < y1) {
    swap(y1, y3);
    swap(x1, x3);
    swap(w1, w3);
  }

  if (y3 < y2) {
    swap(y2, y3);
    swap(x2, x3);
    swap(w2, w3);
  }

  int dy1 = y2 - y1;
  int dx1 = x2 - x1;
  float dw1 = w2 - w1;

  int dy2 = y3 - y1;
  int dx2 = x3 - x1;
  float dw2 = w3 - w1;

  float tex_u, tex_v, tex_w;

  float dax_step = 0, dbx_step = 0, du1_step = 0, dv1_step = 0, du2_step = 0,
        dv2_step = 0, dw1_step = 0, dw2_step = 0;

  if (dy1)
    dax_step = dx1 / (float)abs(dy1);
  if (dy2)
    dbx_step = dx2 / (float)abs(dy2);

  if (dy1)
    dw1_step = dw1 / (float)abs(dy1);
  if (dy2)
    dw2_step = dw2 / (float)abs(dy2);

  if (dy1) {
    for (int i = y1; i <= y2; i++) {
      int ax = x1 + (float)(i - y1) * dax_step;
      int bx = x1 + (float)(i - y1) * dbx_step;

      float tex_sw = w1 + (float)(i - y1) * dw1_step;

      float tex_ew = w1 + (float)(i - y1) * dw2_step;

      if (ax > bx) {
        swap(ax, bx);
        swap(tex_sw, tex_ew);
      }

      tex_w = tex_sw;

      float tstep = 1.0f / ((float)(bx - ax));
      float t = 0.0f;

      int offset;
      for (int j = ax; j < bx; j++) {
        tex_w = (1.0f - t) * tex_sw + t * tex_ew;
        // J is X and I is Y
        offset = i * RI.engine->ScreenWidth() + j;
        if (offset < screenSize && tex_w > RI.depthBuffer[offset]) {
          RI.engine->Draw(j, i, p);
          RI.depthBuffer[offset] = tex_w;
        }
        t += tstep;
      }
    }
  }

  dy1 = y3 - y2;
  dx1 = x3 - x2;
  dw1 = w3 - w2;

  if (dy1)
    dax_step = dx1 / (float)abs(dy1);
  if (dy2)
    dbx_step = dx2 / (float)abs(dy2);

  du1_step = 0, dv1_step = 0;
  if (dy1)
    dw1_step = dw1 / (float)abs(dy1);

  if (dy1) {
    for (int i = y2; i <= y3; i++) {
      int ax = x2 + (float)(i - y2) * dax_step;
      int bx = x1 + (float)(i - y1) * dbx_step;
      float tex_sw = w2 + (float)(i - y2) * dw1_step;
      float tex_ew = w1 + (float)(i - y1) * dw2_step;

      if (ax > bx) {
        swap(ax, bx);
        swap(tex_sw, tex_ew);
      }

      tex_w = tex_sw;

      float tstep = 1.0f / ((float)(bx - ax));
      float t = 0.0f;

      int offset;
      for (int j = ax; j < bx; j++) {
        tex_w = (1.0f - t) * tex_sw + t * tex_ew;
        offset = i * RI.engine->ScreenWidth() + j;
        if (offset < screenSize && tex_w > RI.depthBuffer[offset]) {
          // J is X and I is Y
          RI.engine->Draw(j, i, p);
          RI.depthBuffer[offset] = tex_w;
        }
        t += tstep;
      }
    }
  }
}
