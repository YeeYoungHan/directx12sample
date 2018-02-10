#pragma once

#include "CDirectX12.h"
#include <DirectXMath.h>
#include <memory>

// 정점
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

class CDirectXCubeIndex : public CDirectX12
{
public:
	CDirectXCubeIndex();
	virtual ~CDirectXCubeIndex();

	virtual bool CreateChild();
	virtual bool DrawChild();

	void OnMouseDown( HWND hWnd, WPARAM btnState, int x, int y );
	void OnMouseUp( WPARAM btnState, int x, int y );
	void OnMouseMove( HWND hWnd, WPARAM btnState, int x, int y );

	bool Update();

protected:
	// 정점 버퍼
	Microsoft::WRL::ComPtr<ID3D12Resource> m_pclsUpload;
	int m_iVertexCount;
	POINT m_sttMousePos;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_clsCbvHeap;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_pclsObjectCB;

	float m_fTheta = 1.5f * DirectX::XM_PI;
	float m_fPhi = DirectX::XM_PIDIV4;
	float m_fRadius = 3.0f;

	DirectX::XMFLOAT4X4 m_sttProj = DirectX::XMFLOAT4X4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f );

	bool m_bMouseDown;
};

