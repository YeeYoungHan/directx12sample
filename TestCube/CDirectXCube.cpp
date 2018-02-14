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

#include "stdafx.h"
#include "CDirectXCube.h"


CDirectXCube::CDirectXCube() : m_bMouseDown(false)
{
}


CDirectXCube::~CDirectXCube()
{
}

// 큐브를 그리기 위한 준비 과정
bool CDirectXCube::CreateChild()
{
	// 큐브 정점 정보를 GPU 업로드 힙에 저장한다. - 페이지 246 ~ 248
	Vertex arrCube[] =
	{
		// 앞면 (빨간색)
		{ {  0.5f,  0.5f,  0.5f },{ 1.0f, 0.0f, 0.0f, 1.0f } },
		{ { -0.5f,  0.5f,  0.5f },{ 1.0f, 0.0f, 0.0f, 1.0f } },
		{ { -0.5f, -0.5f,  0.5f },{ 1.0f, 0.0f, 0.0f, 1.0f } },
		{ {  0.5f,  0.5f,  0.5f },{ 1.0f, 0.0f, 0.0f, 1.0f } },
		{ { -0.5f, -0.5f,  0.5f },{ 1.0f, 0.0f, 0.0f, 1.0f } },
		{ {  0.5f, -0.5f,  0.5f },{ 1.0f, 0.0f, 0.0f, 1.0f } },
		
		// 뒷면 (노랑색)
		{ {  0.5f,  0.5f, -0.5f },{ 1.0f, 1.0f, 0.0f, 1.0f } },
		{ {  0.5f, -0.5f, -0.5f },{ 1.0f, 1.0f, 0.0f, 1.0f } },
		{ { -0.5f, -0.5f, -0.5f },{ 1.0f, 1.0f, 0.0f, 1.0f } },
		{ {  0.5f,  0.5f, -0.5f },{ 1.0f, 1.0f, 0.0f, 1.0f } },
		{ { -0.5f, -0.5f, -0.5f },{ 1.0f, 1.0f, 0.0f, 1.0f } },
		{ { -0.5f,  0.5f, -0.5f },{ 1.0f, 1.0f, 0.0f, 1.0f } },

		// 왼쪽 면 (파랑색)
		{ { -0.5f,  0.5f,  0.5f },{ 0.0f, 0.0f, 1.0f, 1.0f } },
		{ { -0.5f, -0.5f, -0.5f },{ 0.0f, 0.0f, 1.0f, 1.0f } },
		{ { -0.5f, -0.5f,  0.5f },{ 0.0f, 0.0f, 1.0f, 1.0f } },
		{ { -0.5f,  0.5f,  0.5f },{ 0.0f, 0.0f, 1.0f, 1.0f } },
		{ { -0.5f,  0.5f, -0.5f },{ 0.0f, 0.0f, 1.0f, 1.0f } },
		{ { -0.5f, -0.5f, -0.5f },{ 0.0f, 0.0f, 1.0f, 1.0f } },

		// 오른쪽 면 (보라색)
		{ {  0.5f,  0.5f,  0.5f },{ 1.0f, 0.0f, 1.0f, 1.0f } },
		{ {  0.5f, -0.5f,  0.5f },{ 1.0f, 0.0f, 1.0f, 1.0f } },
		{ {  0.5f,  0.5f, -0.5f },{ 1.0f, 0.0f, 1.0f, 1.0f } },
		{ {  0.5f,  0.5f, -0.5f },{ 1.0f, 0.0f, 1.0f, 1.0f } },
		{ {  0.5f, -0.5f,  0.5f },{ 1.0f, 0.0f, 1.0f, 1.0f } },
		{ {  0.5f, -0.5f, -0.5f },{ 1.0f, 0.0f, 1.0f, 1.0f } },

		// 윗면 (녹색)
		{ {  0.5f,  0.5f,  0.5f },{ 0.0f, 1.0f, 0.0f, 1.0f } },
		{ {  0.5f,  0.5f, -0.5f },{ 0.0f, 1.0f, 0.0f, 1.0f } },
		{ { -0.5f,  0.5f, -0.5f },{ 0.0f, 1.0f, 0.0f, 1.0f } },
		{ {  0.5f,  0.5f,  0.5f },{ 0.0f, 1.0f, 0.0f, 1.0f } },
		{ { -0.5f,  0.5f, -0.5f },{ 0.0f, 1.0f, 0.0f, 1.0f } },
		{ { -0.5f,  0.5f,  0.5f },{ 0.0f, 1.0f, 0.0f, 1.0f } },

		// 아랫면 (하늘색)
		{ {  0.5f, -0.5f,  0.5f },{ 0.0f, 1.0f, 1.0f, 1.0f } },
		{ { -0.5f, -0.5f, -0.5f },{ 0.0f, 1.0f, 1.0f, 1.0f } },
		{ {  0.5f, -0.5f, -0.5f },{ 0.0f, 1.0f, 1.0f, 1.0f } },
		{ {  0.5f, -0.5f,  0.5f },{ 0.0f, 1.0f, 1.0f, 1.0f } },
		{ { -0.5f, -0.5f,  0.5f },{ 0.0f, 1.0f, 1.0f, 1.0f } },
		{ { -0.5f, -0.5f, -0.5f },{ 0.0f, 1.0f, 1.0f, 1.0f } }
	};

	m_iVertexCount = _countof( arrCube );

	CHECK_FAILED( m_pclsDevice->CreateCommittedResource( &CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_UPLOAD ), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer( sizeof( arrCube ) ),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS( &m_pclsUpload ) ) );

	UINT8 * pDataBegin;
	CD3DX12_RANGE clsRange( 0, 0 );

	// 페이지 270 ~ 271
	CHECK_FAILED( m_pclsUpload->Map( 0, &clsRange, reinterpret_cast<void**>(&pDataBegin) ) );
	memcpy( pDataBegin, arrCube, sizeof( arrCube ) );
	m_pclsUpload->Unmap( 0, nullptr );

	// 정점 셰이더 컴파일 - 페이지 281 ~ 283
	Microsoft::WRL::ComPtr<ID3DBlob> pclsVertexShader;
	Microsoft::WRL::ComPtr<ID3DBlob> pclsPixelShader;

	CHECK_FAILED( D3DCompileFromFile( L"shaders.hlsl", nullptr, nullptr, "VS", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pclsVertexShader, nullptr ) );
	CHECK_FAILED( D3DCompileFromFile( L"shaders.hlsl", nullptr, nullptr, "PS", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pclsPixelShader, nullptr ) );

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

	// 회전을 위한 상수 버퍼를 생성한다.
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = 1;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	CHECK_FAILED( m_pclsDevice->CreateDescriptorHeap( &cbvHeapDesc, IID_PPV_ARGS( &m_clsCbvHeap ) ) );

	CHECK_FAILED( m_pclsDevice->CreateCommittedResource( &CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_UPLOAD ), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer( 256 ),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS( &m_pclsObjectCB ) ) );

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = m_pclsObjectCB->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = 256;

	m_pclsDevice->CreateConstantBufferView( &cbvDesc, m_clsCbvHeap->GetCPUDescriptorHandleForHeapStart() );

	// 원근 감각으로 투영 행렬을 생성한다.
	DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH( 0.25f * DirectX::XM_PI, 1.0f, 1.0f, 1000.0f );
	DirectX::XMStoreFloat4x4( &m_sttProj, P );

	return true;
}

// 큐브를 그린다.
bool CDirectXCube::DrawChild()
{
	ID3D12DescriptorHeap * arrDesc[] = { m_clsCbvHeap.Get() };
	m_pclsCmdList->SetDescriptorHeaps( _countof( arrDesc ), arrDesc );
	m_pclsCmdList->SetGraphicsRootDescriptorTable( 0, m_clsCbvHeap->GetGPUDescriptorHandleForHeapStart() );

	// 정점 버퍼를 파이프라인에 묶는다. - 페이지 250
	D3D12_VERTEX_BUFFER_VIEW clsBufView;
	clsBufView.BufferLocation = m_pclsUpload->GetGPUVirtualAddress();
	clsBufView.StrideInBytes = sizeof( Vertex );
	clsBufView.SizeInBytes = sizeof( Vertex ) * m_iVertexCount;

	m_pclsCmdList->IASetVertexBuffers( 0, 1, &clsBufView );

	// 정점을 그린다. - 페이지 251 ~ 252
	// D3D12 ERROR: ID3D12CommandList::DrawInstanced: Root Parameter Index [0] is not set. On a Resource Binding Tier 2 hardware, all descriptor tables of type CBV and UAV declared in the currently set Root Signature (0x000002D6601C3340:'Unnamed ID3D12RootSignature Object') must be populated, even if the shaders do not need the descriptor. [ EXECUTION ERROR #991: COMMAND_LIST_DESCRIPTOR_TABLE_NOT_SET]
	m_pclsCmdList->DrawInstanced( m_iVertexCount, 1, 0, 0 );

	return true;
}

void CDirectXCube::OnMouseDown( HWND hWnd, WPARAM btnState, int x, int y )
{
	m_sttMousePos.x = x;
	m_sttMousePos.y = y;
	m_bMouseDown = true;

	SetCapture( hWnd );
}

void CDirectXCube::OnMouseUp( WPARAM btnState, int x, int y )
{
	ReleaseCapture();
	m_bMouseDown = false;
}

void CDirectXCube::OnMouseMove( HWND hWnd, WPARAM btnState, int x, int y )
{
	if( m_bMouseDown )
	{
		float dx = 0.005f * static_cast<float>(x - m_sttMousePos.x);
		float dy = 0.005f * static_cast<float>(y - m_sttMousePos.y);

		m_fTheta += dx;
		m_fPhi += dy;

		if( m_fPhi < 0.1f )
		{
			m_fPhi = 0.1f;
		}
		else if( m_fPhi > DirectX::XM_PI )
		{
			m_fPhi = DirectX::XM_PI;
		}

		m_sttMousePos.x = x;
		m_sttMousePos.y = y;

		Update();
		InvalidateRect( hWnd, NULL, TRUE );
	}
}

bool CDirectXCube::Update()
{
	float x = m_fRadius * sinf( m_fPhi ) * cosf( m_fTheta );
	float z = m_fRadius * sinf( m_fPhi ) * sinf( m_fTheta );
	float y = m_fRadius * cosf( m_fPhi );

	// 카메라 위치
	DirectX::XMVECTOR pos = DirectX::XMVectorSet( x, y, z, 1.0f );

	// 큐브 위치
	DirectX::XMVECTOR target = DirectX::XMVectorZero();

	// 카메라가 타겟을 바라볼 때의 위쪽 방향.
	DirectX::XMVECTOR up = DirectX::XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );

	// 카메라 위치와 카메라가 큐브를 바라보는 방향을 이용하여서 View 행렬을 생성한다.
	DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH( pos, target, up );

	DirectX::XMMATRIX proj = DirectX::XMLoadFloat4x4( &m_sttProj );
	DirectX::XMMATRIX worldViewProj = view * proj;

	ConstObject sttConstants;

	XMStoreFloat4x4( &sttConstants.m_sttProj, XMMatrixTranspose( worldViewProj ) );

	UINT8 * pDataBegin;
	CD3DX12_RANGE clsRange( 0, 0 );

	// 페이지 270 ~ 271
	CHECK_FAILED( m_pclsObjectCB->Map( 0, &clsRange, reinterpret_cast<void**>(&pDataBegin) ) );
	memcpy( pDataBegin, &sttConstants, sizeof( sttConstants ) );
	m_pclsObjectCB->Unmap( 0, nullptr );

	return true;
}
