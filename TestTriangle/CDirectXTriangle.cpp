#include "stdafx.h"
#include "CDirectXTriangle.h"


CDirectXTriangle::CDirectXTriangle()
{
}

CDirectXTriangle::~CDirectXTriangle()
{
}

// 삼각형을 그리기 위한 준비 과정
bool CDirectXTriangle::CreateChild()
{
	// 삼각형 정점 정보를 GPU 업로드 힙에 저장한다. - 페이지 246 ~ 248
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

	// 페이지 270 ~ 271
	CHECK_FAILED( m_pclsUpload->Map( 0, &clsRange, reinterpret_cast<void**>(&pDataBegin) ) );
	memcpy( pDataBegin, arrTriangle, sizeof( arrTriangle ) );
	m_pclsUpload->Unmap( 0, nullptr );

	// 정점 셰이더 컴파일 - 페이지 281 ~ 283
	Microsoft::WRL::ComPtr<ID3DBlob> pclsVertexShader;
	Microsoft::WRL::ComPtr<ID3DBlob> pclsPixelShader;

	CHECK_FAILED( D3DCompileFromFile( L"shaders.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pclsVertexShader, nullptr ) );
	CHECK_FAILED( D3DCompileFromFile( L"shaders.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pclsPixelShader, nullptr ) );

	D3D12_INPUT_ELEMENT_DESC arrInputDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	// 파이프라인 상태 객체를 생성한다. - 페이지 290 ~ 295
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

// 삼각형을 그린다.
bool CDirectXTriangle::DrawChild()
{
	// 정점 버퍼를 파이프라인에 묶는다. - 페이지 250
	D3D12_VERTEX_BUFFER_VIEW clsBufView;
	clsBufView.BufferLocation = m_pclsUpload->GetGPUVirtualAddress();
	clsBufView.StrideInBytes = sizeof( Vertex );
	clsBufView.SizeInBytes = sizeof( Vertex ) * 3;

	m_pclsCmdList->IASetVertexBuffers( 0, 1, &clsBufView );

	// 정점을 그린다. - 페이지 251 ~ 252
	m_pclsCmdList->DrawInstanced( 3, 1, 0, 0 );

	return true;
}
