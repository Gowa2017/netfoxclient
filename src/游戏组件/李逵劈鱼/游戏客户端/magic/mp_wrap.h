//-----------------------------------------------------------------------------
// File: mp_wrap.h
//
// Copyright (c) Astralax. All rights reserved.
// Author: Sedov Alexey
//-----------------------------------------------------------------------------

#ifndef MAGIC_PARTICLES_WRAPPER
#define MAGIC_PARTICLES_WRAPPER

#include "mp.h"
#include "hge.h"
#include "hgefont.h"

// eng: Class storing the textural atlas
// rus: ����? ������?������ ���������� ����?
class MP_Atlas_WRAP : public MP_Atlas
{
protected:
	HTEXTURE texture;

	hgeQuad quad;
	
public:
	MP_Atlas_WRAP(int width, int height, const char* file);
	virtual ~MP_Atlas_WRAP(){}

	// eng: Returns texture
	// rus: ���������� ��������
	HTEXTURE GetTexture(){return texture;}

	// eng: Destroy atlas texture
	// rus: ���������� �������� ������
	virtual void Destroy();

	// eng: Loading of frame texture
	// rus: �������� ����������?����?
	virtual void LoadTexture(const MAGIC_CHANGE_ATLAS* c);

	// eng: Cleaning up of rectangle
	// rus: ������?�������������?
	virtual void CleanRectangle(const MAGIC_CHANGE_ATLAS* c);

	// eng: Particle drawing
	// rus: ��������?������?
	virtual void Draw(MAGIC_PARTICLE_VERTEXES* vertexes);

	// eng: Setting of intense
	// rus: ���������� ������������?
	virtual void SetIntense(bool intense);
};

// eng: Class controlling drawing
// rus: ����? ������?��������� ����������
class MP_Device_WRAP : public MP_Device
{
public:
	HGE* hge;
	bool is_lost;

public:

	MP_Device_WRAP(int width, int height, HGE* hge);

	HGE* GetHGE(){return hge;}

	// eng: Creating
	// rus: ��������
	virtual bool Create(){return true;}

	// eng: Destroying
	// rus: ����������?
	virtual void Destroy(){}

	// eng: Beginning of scene drawing
	// rus: ������ ��������?����?
	virtual void BeginScene();

	// eng: End of scene drawing
	// rus: ����?��������?����?
	virtual void EndScene();

	// eng: Indicates that device is lost
	// rus: �������� �� ������ ���������� ���������
	virtual bool IsLost();

	// eng: Loading texture from file
	// rus: �������� �������� �� ����?
	HTEXTURE LoadTextureFromFile(const char* file);
};


#endif