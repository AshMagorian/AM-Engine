#pragma once

class WaterBuffers
{
private:
	unsigned int reflection_gBuffer;
	unsigned int refraction_gBuffer;
	unsigned int reflection_gPosition, reflection_gNormal, reflection_gAlbedo, reflection_gMRA, reflection_rboDepth;
	unsigned int refraction_gPosition, refraction_gNormal, refraction_gAlbedo, refraction_gMRA, refraction_rboDepth;
	unsigned int reflection_fb, refraction_fb;
	unsigned int reflection_tex, reflection_depthTex;
	unsigned int refraction_tex, refraction_depthTex;

	void InitReflectionGBuffer(int w, int h);
	void InitRefractionGBuffer(int w, int h);
	int Init_gPosition(int w, int h, int attatchment);
	int Init_gNormal(int w, int h, int attatchment);
	int Init_gAlbedo(int w, int h, int attatchment);
	int Init_gMRA(int w, int h, int attatchment);
	int Init_RBODepth(int w, int h);

	void InitReflectionFramebuffer();
	void InitRefractionFramebuffer();
	int CreateFramebuffer();
	int CreateTextureAttatchment(int w, int h);
	int CreateDepthTextureAttatchment(int w, int h);
	void BindFramebuffer(int framebuffer, int w, int h);

public:
	WaterBuffers() {}

	void Init(const int& _screenWidth, const int& _screenHeight);

	int REFLECTION_WIDTH = 320;
	int REFLECTION_HEIGHT = 180;
	int REFRACTION_WIDTH = 1280;
	int REFRACTION_HEIGHT = 720;

	void BindReflectionGBuffer();
	void BindRefractionGBuffer();
	void BindReflectionFramebuffer();
	void BindRefractionFrameBuffer();
	void UnbindCurrentFramebuffer(int windowWidth, int windowHeight);

	void BindReflectionGBufferTextures();
	void BindRefractionGBufferTextures();

	void BlitReflectionFrameBuffer();
	void BlitRefractionFrameBuffer();

	int GetReflectionTexture() { return reflection_tex; }
	int GetRefractionTexture() { return refraction_tex; }
	int GetReflectionDepthTexture() { return refraction_rboDepth; }
	int GetRefractionDepthTexture() { return refraction_depthTex; }
};