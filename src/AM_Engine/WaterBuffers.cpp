#include "WaterBuffers.h"
#include "Application.h"
#include "Exception.h"
#include <GL/glew.h>

void WaterBuffers::Init(const int& _screenWidth, const int& _screenHeight)
{
	REFLECTION_WIDTH = _screenWidth / 4;
	REFLECTION_HEIGHT = _screenHeight / 4;
	REFRACTION_WIDTH = _screenWidth;
	REFRACTION_HEIGHT = _screenHeight;
	InitReflectionGBuffer(REFLECTION_WIDTH, REFLECTION_HEIGHT);
	InitRefractionGBuffer(REFRACTION_WIDTH, REFRACTION_HEIGHT);
	InitReflectionFramebuffer();
	InitRefractionFramebuffer();
}

void WaterBuffers::BindReflectionGBuffer()
{
	BindFramebuffer(reflection_gBuffer, REFLECTION_WIDTH, REFLECTION_HEIGHT);
}
void WaterBuffers::BindRefractionGBuffer()
{
	BindFramebuffer(refraction_gBuffer, REFRACTION_WIDTH, REFRACTION_HEIGHT);
}
void WaterBuffers::BindReflectionFramebuffer()
{
	BindFramebuffer(reflection_fb, REFLECTION_WIDTH, REFLECTION_HEIGHT);
}
void WaterBuffers::BindRefractionFrameBuffer()
{
	BindFramebuffer(refraction_fb, REFRACTION_WIDTH, REFRACTION_HEIGHT);
}
void WaterBuffers::BindFramebuffer(int framebuffer, int w, int h)
{
	glBindTexture(GL_TEXTURE_2D, 0);//To make sure the texture isn't bound
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glViewport(0, 0, w, h);
}
void WaterBuffers::UnbindCurrentFramebuffer(int windowWidth, int windowHeight)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, windowWidth, windowHeight);
}
void WaterBuffers::BindReflectionGBufferTextures()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, reflection_gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, reflection_gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, reflection_gAlbedo);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, reflection_gMRA);
}
void WaterBuffers::BindRefractionGBufferTextures()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, refraction_gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, refraction_gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, refraction_gAlbedo);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, refraction_gMRA);
}

void WaterBuffers::BlitReflectionFrameBuffer()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, reflection_gBuffer); // read from the gBuffer
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, reflection_fb); // write to framebuffer
	glBlitFramebuffer(
		0, 0, REFLECTION_WIDTH, REFLECTION_HEIGHT, 0, 0, REFLECTION_WIDTH, REFLECTION_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST
	);
	glBindFramebuffer(GL_FRAMEBUFFER, reflection_fb);
}
void WaterBuffers::BlitRefractionFrameBuffer()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, refraction_gBuffer); // read from the gBuffer
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, refraction_fb); // write to framebuffer
	glBlitFramebuffer(
		0, 0, REFRACTION_WIDTH, REFRACTION_HEIGHT, 0, 0, REFRACTION_WIDTH, REFRACTION_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST
	);
	glBindFramebuffer(GL_FRAMEBUFFER, refraction_fb);
}

void WaterBuffers::InitReflectionGBuffer(int w, int h)
{
	glGenFramebuffers(1, &reflection_gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, reflection_gBuffer);
	reflection_gPosition = Init_gPosition(w, h, GL_COLOR_ATTACHMENT0);
	reflection_gNormal = Init_gNormal(w, h, GL_COLOR_ATTACHMENT1);
	reflection_gAlbedo = Init_gAlbedo(w, h, GL_COLOR_ATTACHMENT2);
	reflection_gMRA = Init_gMRA(w, h, GL_COLOR_ATTACHMENT3);
	// - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, attachments);
	reflection_rboDepth = Init_RBODepth(w, h);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw Exception("gBuffer not Complete!");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void WaterBuffers::InitRefractionGBuffer(int w, int h)
{
	glGenFramebuffers(1, &refraction_gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, refraction_gBuffer);
	refraction_gPosition = Init_gPosition(w, h, GL_COLOR_ATTACHMENT0);
	refraction_gNormal = Init_gNormal(w, h, GL_COLOR_ATTACHMENT1);
	refraction_gAlbedo = Init_gAlbedo(w, h, GL_COLOR_ATTACHMENT2);
	refraction_gMRA = Init_gMRA(w, h, GL_COLOR_ATTACHMENT3);
	// - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, attachments);
	refraction_rboDepth = Init_RBODepth(w, h);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw Exception("gBuffer not Complete!");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int WaterBuffers::Init_gPosition(int w, int h, int attatchment)
{
	unsigned int gPosition;
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attatchment, GL_TEXTURE_2D, gPosition, 0);
	return gPosition;
}
int WaterBuffers::Init_gNormal(int w, int h, int attatchment)
{
	unsigned int gNormal;
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attatchment, GL_TEXTURE_2D, gNormal, 0);
	return gNormal;
}
int WaterBuffers::Init_gAlbedo(int w, int h, int attatchment)
{
	unsigned int gAlbedo;
	glGenTextures(1, &gAlbedo);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attatchment, GL_TEXTURE_2D, gAlbedo, 0);
	return gAlbedo;
}
int WaterBuffers::Init_gMRA(int w, int h, int attatchment)
{
	unsigned int gMRA;
	glGenTextures(1, &gMRA);
	glBindTexture(GL_TEXTURE_2D, gMRA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attatchment, GL_TEXTURE_2D, gMRA, 0);
	return gMRA;
}
int WaterBuffers::Init_RBODepth(int w, int h)
{
	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	return rboDepth;
}


void WaterBuffers::InitReflectionFramebuffer()
{
	reflection_fb = CreateFramebuffer();
	reflection_tex = CreateTextureAttatchment(REFLECTION_WIDTH, REFLECTION_HEIGHT);
	reflection_depthTex = Init_RBODepth(REFLECTION_WIDTH, REFLECTION_HEIGHT);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw Exception("Reflection framebuffer not Complete!");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void WaterBuffers::InitRefractionFramebuffer()
{
	refraction_fb = CreateFramebuffer();
	refraction_tex = CreateTextureAttatchment(REFRACTION_WIDTH, REFRACTION_HEIGHT);
	refraction_depthTex = CreateDepthTextureAttatchment(REFRACTION_WIDTH, REFRACTION_HEIGHT);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw Exception("Refraction framebuffer not Complete!");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
int WaterBuffers::CreateFramebuffer()
{
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	return framebuffer;
}
int WaterBuffers::CreateTextureAttatchment(int w, int h)
{
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	return texture;
}

int WaterBuffers::CreateDepthTextureAttatchment(int w, int h)
{
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D, texture, 0);
	return texture;
}