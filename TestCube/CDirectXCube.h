/*
* Copyright (C) 2016 Yee Young Han <websearch@naver.com> (http://blog.naver.com/websearch)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#pragma once

#include "CDirectX12.h"
#include <DirectXMath.h>
#include <memory>

// ����
struct Vertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT4 Color;
};

struct ConstObject
{
	DirectX::XMFLOAT4X4 m_sttProj = DirectX::XMFLOAT4X4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f );
};

class CDirectXCube : public CDirectX12
{
public:
	CDirectXCube();
	virtual ~CDirectXCube();

	virtual bool CreateChild();
	virtual bool DrawChild();

	void OnMouseDown( HWND hWnd, WPARAM btnState, int x, int y );
	void OnMouseUp( WPARAM btnState, int x, int y );
	void OnMouseMove( HWND hWnd, WPARAM btnState, int x, int y );

	bool Update();

protected:
	// ���� ����
	Microsoft::WRL::ComPtr<ID3D12Resource> m_pclsUpload;
	int m_iVertexCount;
	POINT m_sttMousePos;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_clsCbvHeap;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_pclsObjectCB;

	float m_fTheta = 2.25f * DirectX::XM_PI;
	float m_fPhi = DirectX::XM_PIDIV4;
	float m_fRadius = 3.0f;

	DirectX::XMFLOAT4X4 m_sttProj = DirectX::XMFLOAT4X4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f );

	bool m_bMouseDown;
};

