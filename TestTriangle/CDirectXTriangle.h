#pragma once

#include "CDirectX12.h"

// ����
struct Vertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT4 Color;
};

class CDirectXTriangle : public CDirectX12
{
public:
	CDirectXTriangle();
	virtual ~CDirectXTriangle();

	virtual bool CreateChild();
	virtual bool DrawChild();

protected:
	// ���� ����
	Microsoft::WRL::ComPtr<ID3D12Resource> m_pclsUpload;
};

