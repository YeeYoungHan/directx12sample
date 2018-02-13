#include "stdafx.h"
#include "CDirectXCylinder.h"
#include "trace.h"

CDirectXCylinder::CDirectXCylinder() : m_bMouseDown(false)
{
}


CDirectXCylinder::~CDirectXCylinder()
{
}

// 큐브를 그리기 위한 준비 과정
bool CDirectXCylinder::CreateChild()
{
	m_iVertexCount = m_iVertexColCount * m_iVertexRowCount + 2;
	m_iIndexCount = 6 * m_iVertexColCount * ( m_iVertexRowCount - 1 ) + m_iVertexColCount * 3 * 2;

	CHECK_FAILED( m_pclsDevice->CreateCommittedResource( &CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_UPLOAD ), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer( m_iVertexCount * sizeof( Vertex ) ),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS( &m_pclsVertexBuf ) ) );
	CHECK_FAILED( m_pclsDevice->CreateCommittedResource( &CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_UPLOAD ), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer( m_iIndexCount * sizeof( uint16_t ) ),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS( &m_pclsIndexBuf ) ) );

	// ===========================================================
	// 정점을 저장한다.
	Vertex * pVertex;
	CD3DX12_RANGE clsRange( 0, 0 );

	CHECK_FAILED( m_pclsVertexBuf->Map( 0, &clsRange, reinterpret_cast<void**>(&pVertex) ) );

	// 테두리 정점들
	for( int r = 0; r < m_iVertexRowCount; ++r )
	{
		for( int c = 0; c < m_iVertexColCount; ++c )
		{
			pVertex->Pos.x = sinf( DirectX::XM_2PI * c / m_iVertexColCount ) * m_fVertexRadius;
			pVertex->Pos.y = (m_iVertexRowCount / 2 - r) * m_fVertexRowHeight;
			pVertex->Pos.z = cosf( DirectX::XM_2PI * c / m_iVertexColCount ) * m_fVertexRadius * (-1);

			pVertex->Color.w = 1.0;
			pVertex->Color.x = pVertex->Pos.x / m_fVertexRadius;
			pVertex->Color.y = (float)r / m_iVertexRowCount;
			pVertex->Color.z = pVertex->Pos.z / m_fVertexRadius;

			++pVertex;
		}
	}

	// 상판 중심
	pVertex->Pos.x = 0;
	pVertex->Pos.y = m_iVertexRowCount / 2 * m_fVertexRowHeight;
	pVertex->Pos.z = 0;

	pVertex->Color.w = 1.0;
	pVertex->Color.x = 1.0;
	pVertex->Color.y = 0;
	pVertex->Color.z = 0;

	++pVertex;

	// 하판 중심
	pVertex->Pos.x = 0;
	pVertex->Pos.y = m_iVertexRowCount / 2 * m_fVertexRowHeight * (-1);
	pVertex->Pos.z = 0;

	pVertex->Color.w = 1.0;
	pVertex->Color.x = 1.0;
	pVertex->Color.y = 0;
	pVertex->Color.z = 0;

	m_pclsVertexBuf->Unmap( 0, nullptr );

	// ===========================================================
	// 정점 인덱스를 저장한다.
	uint16_t * pIndex;

	CHECK_FAILED( m_pclsIndexBuf->Map( 0, &clsRange, reinterpret_cast<void**>(&pIndex) ) );

	// 테두리 정점 인덱스
	for( int r = 0; r < ( m_iVertexRowCount - 1); ++r )
	{
		for( int c = 0; c < m_iVertexColCount; ++c )
		{
			bool bEnd = false;

			if( (c + 1) == m_iVertexColCount ) bEnd = true;

			// 첫번째 삼각형
			*pIndex = r * m_iVertexColCount + c;
			++pIndex;

			if( bEnd )
			{
				*pIndex = r * m_iVertexColCount;
			}
			else
			{
				*pIndex = r * m_iVertexColCount + c + 1;
			}
			++pIndex;

			*pIndex = (r + 1) * m_iVertexColCount + c;
			++pIndex;

			// 두번째 삼각형
			if( bEnd )
			{
				*pIndex = r * m_iVertexColCount;
			}
			else
			{
				*pIndex = r * m_iVertexColCount + c + 1;
			}
			++pIndex;

			if( bEnd )
			{
				*pIndex = (r + 1) * m_iVertexColCount;
			}
			else
			{
				*pIndex = (r + 1) * m_iVertexColCount + c + 1;
			}
			++pIndex;

			*pIndex = (r + 1) * m_iVertexColCount + c;
			++pIndex;
		}
	}

	// 상판 인덱스
	uint16_t sIndex = m_iVertexRowCount * m_iVertexColCount;
	uint16_t sLastRowIndex = (m_iVertexRowCount - 1 ) * m_iVertexColCount;

	for( int c = 0; c < m_iVertexColCount; ++c )
	{
		*pIndex = c;
		++pIndex;

		*pIndex = sIndex;
		++pIndex;

		if( (c + 1) == m_iVertexColCount )
		{
			*pIndex = 0;
		}
		else
		{
			*pIndex = c + 1;
		}
		++pIndex;
	}

	// 하판 인덱스
	for( int c = 0; c < m_iVertexColCount; ++c )
	{
		*pIndex = sIndex + 1;
		++pIndex;

		*pIndex = c + sLastRowIndex;
		++pIndex;

		if( (c + 1) == m_iVertexColCount )
		{
			*pIndex = sLastRowIndex;
		}
		else
		{
			*pIndex = c + sLastRowIndex + 1;
		}
		++pIndex;
	}
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
	//psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
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
bool CDirectXCylinder::DrawChild()
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

void CDirectXCylinder::OnMouseDown( HWND hWnd, WPARAM btnState, int x, int y )
{
	m_sttMousePos.x = x;
	m_sttMousePos.y = y;
	m_bMouseDown = true;

	SetCapture( hWnd );
}

void CDirectXCylinder::OnMouseUp( WPARAM btnState, int x, int y )
{
	ReleaseCapture();
	m_bMouseDown = false;
}

void CDirectXCylinder::OnMouseMove( HWND hWnd, WPARAM btnState, int x, int y )
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

bool CDirectXCylinder::Update()
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
