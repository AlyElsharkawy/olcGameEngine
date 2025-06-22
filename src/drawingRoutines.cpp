#include "drawingRoutines.h"
#include "miscPrimitives.h"
#include <X11/Xlib.h>

void DrawTexturedTriangle(const RenderingInstance& RI, const Triangle& input, const olc::Sprite* texture)
{

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

		if (y2 < y1)
		{
			swap(y1, y2);
			swap(x1, x2);
			swap(u1, u2);
			swap(v1, v2);
			swap(w1, w2);
		}

		if (y3 < y1)
		{
			swap(y1, y3);
			swap(x1, x3);
			swap(u1, u3);
			swap(v1, v3);
			swap(w1, w3);
		}

		if (y3 < y2)
		{
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

		float dax_step = 0, dbx_step = 0,
			du1_step = 0, dv1_step = 0,
			du2_step = 0, dv2_step = 0,
			dw1_step=0, dw2_step=0;

		if (dy1) dax_step = dx1 / (float)abs(dy1);
		if (dy2) dbx_step = dx2 / (float)abs(dy2);

		if (dy1) du1_step = du1 / (float)abs(dy1);
		if (dy1) dv1_step = dv1 / (float)abs(dy1);
		if (dy1) dw1_step = dw1 / (float)abs(dy1);

		if (dy2) du2_step = du2 / (float)abs(dy2);
		if (dy2) dv2_step = dv2 / (float)abs(dy2);
		if (dy2) dw2_step = dw2 / (float)abs(dy2);

		if (dy1)
		{
			for (int i = y1; i <= y2; i++)
			{
				int ax = x1 + (float)(i - y1) * dax_step;
				int bx = x1 + (float)(i - y1) * dbx_step;

				float tex_su = u1 + (float)(i - y1) * du1_step;
				float tex_sv = v1 + (float)(i - y1) * dv1_step;
				float tex_sw = w1 + (float)(i - y1) * dw1_step;

				float tex_eu = u1 + (float)(i - y1) * du2_step;
				float tex_ev = v1 + (float)(i - y1) * dv2_step;
				float tex_ew = w1 + (float)(i - y1) * dw2_step;

				if (ax > bx)
				{
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

				for (int j = ax; j < bx; j++)
				{
					tex_u = (1.0f - t) * tex_su + t * tex_eu;
					tex_v = (1.0f - t) * tex_sv + t * tex_ev;
					tex_w = (1.0f - t) * tex_sw + t * tex_ew;
          //J is X and I is Y
					if (tex_w < RI.depthBuffer[i*RI.engine->ScreenWidth() + j])
					{
                        RI.engine->Draw(j, i, texture->Sample(tex_u / tex_w, tex_v / tex_w));
						RI.depthBuffer[i*RI.engine->ScreenWidth() + j] = tex_w;
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

		if (dy1) dax_step = dx1 / (float)abs(dy1);
		if (dy2) dbx_step = dx2 / (float)abs(dy2);

		du1_step = 0, dv1_step = 0;
		if (dy1) du1_step = du1 / (float)abs(dy1);
		if (dy1) dv1_step = dv1 / (float)abs(dy1);
		if (dy1) dw1_step = dw1 / (float)abs(dy1);

		if (dy1)
		{
			for (int i = y2; i <= y3; i++)
			{
				int ax = x2 + (float)(i - y2) * dax_step;
				int bx = x1 + (float)(i - y1) * dbx_step;

				float tex_su = u2 + (float)(i - y2) * du1_step;
				float tex_sv = v2 + (float)(i - y2) * dv1_step;
				float tex_sw = w2 + (float)(i - y2) * dw1_step;

				float tex_eu = u1 + (float)(i - y1) * du2_step;
				float tex_ev = v1 + (float)(i - y1) * dv2_step;
				float tex_ew = w1 + (float)(i - y1) * dw2_step;

				if (ax > bx)
				{
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

				for (int j = ax; j < bx; j++)
				{
					tex_u = (1.0f - t) * tex_su + t * tex_eu;
					tex_v = (1.0f - t) * tex_sv + t * tex_ev;
					tex_w = (1.0f - t) * tex_sw + t * tex_ew;

					if (tex_w < RI.depthBuffer[i*RI.engine->ScreenWidth() + j])
					{
            //J is X and I is Y
						RI.engine->Draw(j, i, texture->Sample(tex_u / tex_w, tex_v / tex_w));
						RI.depthBuffer[i*RI.engine->ScreenWidth() + j] = tex_w;
					}
					t += tstep;
				}
			}	
		}
}

void DrawTriangleWithDepthBuffer(const Triangle& triangleInput, const RenderingInstance& RI, olc::Pixel p)
{
    DrawTriangleWithDepthBufferInline(triangleInput.points[0].x, triangleInput.points[0].y, triangleInput.points[0].z,
                                triangleInput.points[1].x, triangleInput.points[1].y, triangleInput.points[1].z,
                                triangleInput.points[2].x, triangleInput.points[2].y, triangleInput.points[2].z,
                                RI,
                                p);
}

void DrawTriangleWithDepthBufferInline(int32_t x1, int32_t y1, float z1,
                                 int32_t x2, int32_t y2, float z2,
                                 int32_t x3, int32_t y3, float z3,
                                 const RenderingInstance& RI,
                                 olc::Pixel p)
{
    if (y2 < y1)
    {
        swap(y1, y2);
        swap(x1, x2);
        swap(z1, z2);
    }

    if (y3 < y1)
    {
        swap(y1, y3);
        swap(x1, x3);
        swap(z1, z3);
    }

    if (y3 < y2)
    {
	    swap(y2, y3);
	    swap(x2, x3);
        swap(z2, z3);
    }

    int dy1 = y2 - y1;
    int dx1 = x2 - x1;
    int dz1 = z2 - z1;

    int dy2 = y3 - y1;
    int dx2 = x3 - x1;
    int dz2 = z3 - z1;
    
    float tex_z;
    float dax_step = 0, dbx_step = 0, 
          dz1_step = 0, dz2_step;

    if (dy1) dax_step = dx1 / (float)abs(dy1);
    if (dy2) dbx_step = dx2 / (float)abs(dy2);
    
    if(dy1) dz1_step = dz1 / (float)abs(dy1);
    if(dy2) dz2_step = dz2 / (float)abs(dy2);

    if (dy1)
    {
        for (int i = y1; i <= y2; i++)
        {
            int ax = x1 + (float)(i - y1) * dax_step;
            int bx = x1 + (float)(i - y1) * dbx_step;
            
            float tex_sz = z1 + (float)(i - y1) * dz1_step;
            float tex_ez = z1 + (float)(i - y1) * dz2_step;
            if (ax > bx)
            {
                swap(ax, bx);
                swap(tex_sz, tex_ez);
            }
            if(tex_sz <= RI.depthBuffer[RI.engine->ScreenWidth() * i + ax])
            {
                RI.engine->Draw(ax, i, p);
                RI.depthBuffer[RI.engine->ScreenWidth() * i + ax] = tex_sz;
            }
            
            if(tex_ez <= RI.depthBuffer[RI.engine->ScreenWidth() * i + bx])
            {
                RI.engine->Draw(bx, i, p);
                RI.depthBuffer[RI.engine->ScreenWidth() * i + bx] = tex_ez;
            }
        }
    }

    dy1 = y3 - y2;
    dx1 = x3 - x2;
    dz1 = z3 - z2;

    if (dy1) dax_step = dx1 / (float)abs(dy1);
    if (dy2) dbx_step = dx2 / (float)abs(dy2);

    dz1_step = 0;
    if (dy1) dz1_step = dz1 / (float)abs(dy1);

    if (dy1)
    {
        for (int i = y2; i <= y3; i++)
        {
            int ax = x2 + (float)(i - y2) * dax_step;
            int bx = x1 + (float)(i - y1) * dbx_step;
            
            float tex_sz = z2 + (float)(i - y2) * dz1_step;
            float tex_ez = z1 + (float)(i - y1) * dz2_step;
            if (ax > bx)
            {
                swap(ax, bx);
                swap(tex_sz, tex_ez);
            }

            if(tex_sz <= RI.depthBuffer[RI.engine->ScreenWidth() * i + ax])
            {
                RI.engine->Draw(ax, i, p);
                RI.depthBuffer[RI.engine->ScreenWidth() * i + ax] = tex_sz;
            }
            
            if(tex_ez <= RI.depthBuffer[RI.engine->ScreenWidth() * i + bx])
            {
                RI.engine->Draw(bx, i, p);
                RI.depthBuffer[RI.engine->ScreenWidth() * i + bx] = tex_ez;
            }
        }
    }
}

//WARNING: This implementation will leave "holes" while drawing triangle. I will fix this later
void FillTriangleWithDepthBuffer(const Triangle& triangleInput, const RenderingInstance& RI, olc::Pixel p)
{
    FillTriangleWithDepthBufferInline(triangleInput.points[0].x, triangleInput.points[0].y, triangleInput.points[0].z,
                                triangleInput.points[1].x, triangleInput.points[1].y, triangleInput.points[1].z, 
                                triangleInput.points[2].x, triangleInput.points[2].y, triangleInput.points[2].z,
                                RI,
                                p);
}

void FillTriangleWithDepthBufferInline(int32_t x1, int32_t y1, float z1,
                                 int32_t x2, int32_t y2, float z2, 
                                 int32_t x3, int32_t y3, float z3,
                                 const RenderingInstance& RI,
                                 olc::Pixel p)
{
    if (y2 < y1)
    {
        swap(y1, y2);
        swap(x1, x2);
        swap(z1, z2);
    }

    if (y3 < y1)
    {
        swap(y1, y3);
        swap(x1, x3);
        swap(z1, z3);
    }

    if (y3 < y2)
    {
	    swap(y2, y3);
	    swap(x2, x3);
        swap(z2, z3);
    }

    int dy1 = y2 - y1;
    int dx1 = x2 - x1;
    int dz1 = z2 - z1;

    int dy2 = y3 - y1;
    int dx2 = x3 - x1;
    int dz2 = z3 - z1;
    
    float tex_z;
    float dax_step = 0, dbx_step = 0, 
          dz1_step = 0, dz2_step;

    if (dy1) dax_step = dx1 / (float)abs(dy1);
    if (dy2) dbx_step = dx2 / (float)abs(dy2);
    
    if(dy1) dz1_step = dz1 / (float)abs(dy1);
    if(dy2) dz2_step = dz2 / (float)abs(dy2);

    if (dy1)
    {
        for (int i = y1; i <= y2; i++)
        {
            int ax = x1 + (float)(i - y1) * dax_step;
            int bx = x1 + (float)(i - y1) * dbx_step;
            
            float tex_sz = z1 + (float)(i - y1) * dz1_step;
            float tex_ez = z1 + (float)(i - y1) * dz2_step;
            if (ax > bx)
            {
                swap(ax, bx);
                swap(tex_sz, tex_ez);
            }

            tex_z = tex_sz;
            float tstep = 1.0f / ((float)(bx - ax));
            float t = 0.0f;

            for (int j = ax; j < bx; j++)
            {
                //J is X and I is Y
                tex_z = (1.0f - t) * tex_sz + t * tex_ez;
                if (tex_z < RI.depthBuffer[i*RI.engine->ScreenWidth() + j])
                {
                    RI.engine->Draw(j, i, p);
                    RI.depthBuffer[i*RI.engine->ScreenWidth() + j] = tex_z;
                }
                t += tstep;
            }
        }
    }

    dy1 = y3 - y2;
    dx1 = x3 - x2;
    dz1 = z3 - z2;

    if (dy1) dax_step = dx1 / (float)abs(dy1);
    if (dy2) dbx_step = dx2 / (float)abs(dy2);

    dz1_step = 0;
    if (dy1) dz1_step = dz1 / (float)abs(dy1);

    if (dy1)
    {
        for (int i = y2; i <= y3; i++)
        {
            int ax = x2 + (float)(i - y2) * dax_step;
            int bx = x1 + (float)(i - y1) * dbx_step;
            
            float tex_sz = z2 + (float)(i - y2) * dz1_step;
            float tex_ez = z1 + (float)(i - y1) * dz2_step;
            if (ax > bx)
            {
                swap(ax, bx);
                swap(tex_sz, tex_ez);
            }

            tex_z = tex_sz;
            float tstep = 1.0f / ((float)(bx - ax));
            float t = 0.0f;

            for (int j = ax; j < bx; j++)
            {
                //J is X and I is Y
                tex_z = (1.0f - t) * tex_sz + t * tex_ez;
                if (tex_z < RI.depthBuffer[i*RI.engine->ScreenWidth() + j])
                {
                    RI.engine->Draw(j, i, p);
                    RI.depthBuffer[i*RI.engine->ScreenWidth() + j] = tex_z;
                }
                t += tstep;
            }
        }
    }
}

