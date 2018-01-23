#include "CDirectX12.h"
#include <comdef.h>
#include <tchar.h>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

CDirectX12::CDirectX12()
{
}


CDirectX12::~CDirectX12()
{
}

/**
 * @ingroup LibDirectX12
 * @brief DirectX12 ����� ���� �������� �����Ѵ�.
 * @param hWnd	������ �ڵ�
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CDirectX12::Create( HWND hWnd )
{
#ifdef _DEBUG
	// ����� �α� Ȱ��ȭ
	{
		Microsoft::WRL::ComPtr<ID3D12Debug> pclsDebug;
		CHECK_FAILED( D3D12GetDebugInterface( IID_PPV_ARGS( &pclsDebug ) ) );
		pclsDebug->EnableDebugLayer();
	}
#endif

	// DXGIFactory, Device, Fence ����
	CHECK_FAILED( CreateDXGIFactory1( IID_PPV_ARGS( &m_pclsFactory ) ) );
	CHECK_FAILED( D3D12CreateDevice( nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS( &m_pclsDevice ) ) );
	CHECK_FAILED( m_pclsDevice->CreateFence( 0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( &m_pclsFence ) ) );

	// ��� ��⿭, �޸� �Ҵ���, ��� ��� ��ü ����
	D3D12_COMMAND_QUEUE_DESC clsQueueDesc = {};
	clsQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	clsQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	CHECK_FAILED( m_pclsDevice->CreateCommandQueue( &clsQueueDesc, IID_PPV_ARGS( &m_pclsCmdQueue ) ) );

	CHECK_FAILED( m_pclsDevice->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( &m_pclsCmdAlloc ) ) );
	CHECK_FAILED( m_pclsDevice->CreateCommandList( 0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pclsCmdAlloc.Get(), nullptr, IID_PPV_ARGS( &m_pclsCmdList ) ) );
	m_pclsCmdList->Close();

	// ��ȯ �罽 ��ü ����
	RECT clsRect;
	int iWidth, iHeight;

	if( GetClientRect( hWnd, &clsRect ) )
	{
		iWidth = clsRect.right - clsRect.left;
		iHeight = clsRect.bottom - clsRect.top;
	}

	DXGI_SWAP_CHAIN_DESC clsSwapDesc;
	clsSwapDesc.BufferDesc.Width = iWidth;
	clsSwapDesc.BufferDesc.Height = iHeight;
	clsSwapDesc.BufferDesc.RefreshRate.Numerator = 60;
	clsSwapDesc.BufferDesc.RefreshRate.Denominator = 1;
	clsSwapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	clsSwapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	clsSwapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	clsSwapDesc.SampleDesc.Count = 1;
	clsSwapDesc.SampleDesc.Quality = 0;
	clsSwapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	clsSwapDesc.BufferCount = SWAP_CHAIN_BUF_COUNT;
	clsSwapDesc.OutputWindow = hWnd;
	clsSwapDesc.Windowed = true;
	clsSwapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	clsSwapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	CHECK_FAILED( m_pclsFactory->CreateSwapChain( m_pclsCmdQueue.Get(), &clsSwapDesc, &m_pclsSwapChain ) );

	// RTV( Render Target View ) �� ���� ������ �� �� RTV ����
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = SWAP_CHAIN_BUF_COUNT;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	CHECK_FAILED( m_pclsDevice->CreateDescriptorHeap( &rtvHeapDesc, IID_PPV_ARGS( &m_pclsRtvHeap ) ) );

	D3D12_CPU_DESCRIPTOR_HANDLE pclsRtvHeapPos = m_pclsRtvHeap->GetCPUDescriptorHandleForHeapStart();
	UINT iRtvSize = m_pclsDevice->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_RTV );
	for( UINT i = 0; i < SWAP_CHAIN_BUF_COUNT; i++ )
	{
		CHECK_FAILED( m_pclsSwapChain->GetBuffer( i, IID_PPV_ARGS( &m_pclsRtvBuf[i] ) ) );
		m_pclsDevice->CreateRenderTargetView( m_pclsRtvBuf[i].Get(), nullptr, pclsRtvHeapPos );
		pclsRtvHeapPos.ptr += iRtvSize;
	}

	return true;
}

/**
 * @ingroup LibDirectX12
 * @brief GPU ����� �Ϸ�� ������ ����Ѵ�.
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CDirectX12::WaitCmdQueue()
{
	++m_iFence;

	CHECK_FAILED( m_pclsCmdQueue->Signal( m_pclsFence.Get(), m_iFence ) );

	if( m_pclsFence->GetCompletedValue() < m_iFence )
	{
		HANDLE hEvent = CreateEventEx( nullptr, false, false, EVENT_ALL_ACCESS );

		CHECK_FAILED( m_pclsFence->SetEventOnCompletion( m_iFence, hEvent ) );

		WaitForSingleObject( hEvent, INFINITE );
		CloseHandle( hEvent );
	}

	return true;
}

/**
 * @ingroup LibDirectX12
 * @brief DirectX12 �Լ� ���� �߻��� ���� ���� ���ڿ��� �����´�.
 * @returns DirectX12 �Լ� ���� �߻��� ���� ���� ���ڿ��� �����Ѵ�.
 */
const TCHAR * CDirectX12::GetErrString()
{
	_com_error clsError( m_iErrCode );
	static std::wstring strMsg = _T( "hr=" ) + std::to_wstring( m_iErrCode )
		+ _T( ";\r\nmsg=" ) + clsError.ErrorMessage()
		+ _T( ";\r\nfile=" ) + m_strErrFile
		+ _T( ";\r\nline=" ) + std::to_wstring( m_iErrLine )
		+ _T( ";\r\nfunc=" ) + m_strErrFunc;

	return strMsg.c_str();
}
