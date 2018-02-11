#include "stdafx.h"
#include "CDirectXCubeIndex.h"


CDirectXCubeIndex::CDirectXCubeIndex() : m_bMouseDown(false)
{
}


CDirectXCubeIndex::~CDirectXCubeIndex()
{
}

// 큐브를 그리기 위한 준비 과정
bool CDirectXCubeIndex::CreateChild()
{
	// 큐브 정점 정보를 GPU 업로드 힙에 저장한다. - 페이지 246 ~ 248
	Vertex arrVertex[] =
	{
		{ {  0.5f,  0.5f,  0.5f },{ 1.0f, 0.0f, 0.0f, 1.0f } },	// index 0
		{ {  0.5f, -0.5f,  0.5f },{ 0.0f, 1.0f, 0.0f, 1.0f } },	// index 1
		{ { -0.5f, -0.5f,  0.5f },{ 0.0f, 0.0f, 1.0f, 1.0f } },	// index 2
		{ { -0.5f,  0.5f,  0.5f },{ 1.0f, 1.0f, 0.0f, 1.0f } },	// index 3
		{ {  0.5f,  0.5f, -0.5f },{ 1.0f, 0.0f, 1.0f, 1.0f } },	// index 4
		{ {  0.5f, -0.5f, -0.5f },{ 0.0f, 1.0f, 1.0f, 1.0f } },	// index 5
		{ { -0.5f, -0.5f, -0.5f },{ 0.5f, 0.5f, 0.0f, 1.0f } },	// index 6
		{ { -0.5f,  0.5f, -0.5f },{ 0.0f, 0.5f, 0.5f, 1.0f } },	// index 7
	};

	uint16_t arrIndex[] =
	{
		// 앞면
		0, 2, 1,
		0, 3, 2,

		// 뒷면
		4, 6, 7,
		4, 5, 6,

		// 왼쪽면
		3, 6, 2,
		3, 7, 6,

		// 오른쪽면
		0, 1, 5,
		0, 5, 4,

		// 윗면
		0, 4, 7,
		0, 7, 3,

		// 아랫면
		1, 6, 5,
		1, 2, 6
	};

	m_iVertexCount = _countof( arrVertex );
	m_iIndexCount = _countof( arrIndex );

	CHECK_FAILED( m_pclsDevice->CreateCommittedResource( &CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_UPLOAD ), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer( sizeof( arrVertex ) ),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS( &m_pclsVertexBuf ) ) );
	CHECK_FAILED( m_pclsDevice->CreateCommittedResource( &CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_UPLOAD ), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer( sizeof( arrIndex ) ),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS( &m_pclsIndexBuf ) ) );

	UINT8 * pDataBegin;
	CD3DX12_RANGE clsRange( 0, 0 );

	// 페이지 270 ~ 271
	CHECK_FAILED( m_pclsVertexBuf->Map( 0, &clsRange, reinterpret_cast<void**>(&pDataBegin) ) );
	memcpy( pDataBegin, arrVertex, sizeof( arrVertex ) );
	m_pclsVertexBuf->Unmap( 0, nullptr );

	CHECK_FAILED( m_pclsIndexBuf->Map( 0, &clsRange, reinterpret_cast<void**>(&pDataBegin) ) );
	memcpy( pDataBegin, arrIndex, sizeof( arrIndex ) );
	m_pclsIndexBuf->Unmap( 0, nullptr );

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
bool CDirectXCubeIndex::DrawChild()
{
	ID3D12DescriptorHeap * arrDesc[] = { m_clsCbvHeap.Get() };
	m_pclsCmdList->SetDescriptorHeaps( _countof( arrDesc ), arrDesc );
	m_pclsCmdList->SetGraphicsRootDescriptorTable( 0, m_clsCbvHeap->GetGPUDescriptorHandleForHeapStart() );

	// 정점 버퍼를 파이프라인에 묶는다. - 페이지 250
	D3D12_VERTEX_BUFFER_VIEW clsVertexBufView;
	clsVertexBufView.BufferLocation = m_pclsVertexBuf->GetGPUVirtualAddress();
	clsVertexBufView.StrideInBytes = sizeof( Vertex );
	clsVertexBufView.SizeInBytes = sizeof( Vertex ) * m_iVertexCount;

	m_pclsCmdList->IASetVertexBuffers( 0, 1, &clsVertexBufView );

	// 인덱스를 파이프라인에 묶는다.
	D3D12_INDEX_BUFFER_VIEW clsIndexBufView;
	clsIndexBufView.BufferLocation = m_pclsIndexBuf->GetGPUVirtualAddress();
	clsIndexBufView.Format = DXGI_FORMAT_R16_UINT;
	clsIndexBufView.SizeInBytes = sizeof(uint16_t) * m_iIndexCount;

	m_pclsCmdList->IASetIndexBuffer( &clsIndexBufView );

	// 정점을 그린다.
	m_pclsCmdList->DrawIndexedInstanced( m_iIndexCount, 1, 0, 0, 0 );

	return true;
}

void CDirectXCubeIndex::OnMouseDown( HWND hWnd, WPARAM btnState, int x, int y )
{
	m_sttMousePos.x = x;
	m_sttMousePos.y = y;
	m_bMouseDown = true;

	SetCapture( hWnd );
}

void CDirectXCubeIndex::OnMouseUp( WPARAM btnState, int x, int y )
{
	ReleaseCapture();
	m_bMouseDown = false;
}

void CDirectXCubeIndex::OnMouseMove( HWND hWnd, WPARAM btnState, int x, int y )
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

bool CDirectXCubeIndex::Update()
{
	float x = m_fRadius * sinf( m_fPhi ) * cosf( m_fTheta );
	float z = m_fRadius * sinf( m_fPhi ) * sinf( m_fTheta );
	float y = m_fRadius * cosf( m_fPhi );

	TRACE( "Phi(%.4f) Theta(%.4f) x(%.4f) y(%.4f) z(%.4f)\n", m_fPhi, m_fTheta, x, y, z );

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
