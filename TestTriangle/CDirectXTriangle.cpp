#include "stdafx.h"
#include "CDirectXTriangle.h"


CDirectXTriangle::CDirectXTriangle()
{
}

CDirectXTriangle::~CDirectXTriangle()
{
}

// �ﰢ���� �׸��� ���� �غ� ����
bool CDirectXTriangle::CreateChild()
{
	// �ﰢ�� ���� ������ GPU ���ε� ���� �����Ѵ�. - ������ 246 ~ 248
	Vertex arrTriangle[] =
	{
		{ { 0.0f, 0.25f, 0.0f },{ 1.0f, 0.0f, 0.0f, 1.0f } },
		{ { 0.25f, -0.25f, 0.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } },
		{ { -0.25f, -0.25f, 0.0f },{ 0.0f, 0.0f, 1.0f, 1.0f } }
	};

	CHECK_FAILED( m_pclsDevice->CreateCommittedResource( &CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_UPLOAD ), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer( sizeof( arrTriangle ) ),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS( &m_pclsUpload ) ) );

	UINT8 * pDataBegin;
	CD3DX12_RANGE clsRange( 0, 0 );

	// ������ 270 ~ 271
	CHECK_FAILED( m_pclsUpload->Map( 0, &clsRange, reinterpret_cast<void**>(&pDataBegin) ) );
	memcpy( pDataBegin, arrTriangle, sizeof( arrTriangle ) );
	m_pclsUpload->Unmap( 0, nullptr );

	// ���� ���̴� ������ - ������ 281 ~ 283
	Microsoft::WRL::ComPtr<ID3DBlob> pclsVertexShader;
	Microsoft::WRL::ComPtr<ID3DBlob> pclsPixelShader;

	CHECK_FAILED( D3DCompileFromFile( L"shaders.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pclsVertexShader, nullptr ) );
	CHECK_FAILED( D3DCompileFromFile( L"shaders.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pclsPixelShader, nullptr ) );

	D3D12_INPUT_ELEMENT_DESC arrInputDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	// ���������� ���� ��ü�� �����Ѵ�. - ������ 290 ~ 295
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { arrInputDesc, _countof( arrInputDesc ) };
	psoDesc.pRootSignature = m_pclsRootSignature.Get();
	psoDesc.VS = CD3DX12_SHADER_BYTECODE( pclsVertexShader.Get() );
	psoDesc.PS = CD3DX12_SHADER_BYTECODE( pclsPixelShader.Get() );
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC( D3D12_DEFAULT );
	psoDesc.BlendState = CD3DX12_BLEND_DESC( D3D12_DEFAULT );
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;
	CHECK_FAILED( m_pclsDevice->CreateGraphicsPipelineState( &psoDesc, IID_PPV_ARGS( &m_pclsPipeLineState ) ) );

	return true;
}

// �ﰢ���� �׸���.
bool CDirectXTriangle::DrawChild()
{
	// ���� ���۸� ���������ο� ���´�. - ������ 250
	D3D12_VERTEX_BUFFER_VIEW clsBufView;
	clsBufView.BufferLocation = m_pclsUpload->GetGPUVirtualAddress();
	clsBufView.StrideInBytes = sizeof( Vertex );
	clsBufView.SizeInBytes = sizeof( Vertex ) * 3;

	m_pclsCmdList->IASetVertexBuffers( 0, 1, &clsBufView );

	// ������ �׸���. - ������ 251 ~ 252
	m_pclsCmdList->DrawInstanced( 3, 1, 0, 0 );

	return true;
}
