#pragma once

#include "CDirectX12.h"

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

	bool CreateChild();
	bool DrawChild();

protected:
	Microsoft::WRL::ComPtr<ID3D12Resource> m_pclsUpload;
};

