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

	// ��ġ ���� - DXGIFactory, Device, Fence ����. ������ 138 ~ 140
	CHECK_FAILED( CreateDXGIFactory1( IID_PPV_ARGS( &m_pclsFactory ) ) );
	CHECK_FAILED( D3D12CreateDevice( nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS( &m_pclsDevice ) ) );
	CHECK_FAILED( m_pclsDevice->CreateFence( 0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( &m_pclsFence ) ) );

	// ��� ��⿭, �޸� �Ҵ���, ��� ��� ��ü ����. ������ 141 ~ 142
	D3D12_COMMAND_QUEUE_DESC clsQueueDesc = {};
	clsQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	clsQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	CHECK_FAILED( m_pclsDevice->CreateCommandQueue( &clsQueueDesc, IID_PPV_ARGS( &m_pclsCmdQueue ) ) );

	CHECK_FAILED( m_pclsDevice->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( &m_pclsCmdAlloc ) ) );
	CHECK_FAILED( m_pclsDevice->CreateCommandList( 0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pclsCmdAlloc.Get(), nullptr, IID_PPV_ARGS( &m_pclsCmdList ) ) );
	m_pclsCmdList->Close();

	// ��ȯ �罽 ��ü ����. ������ 142 ~ 145
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

	// RTV( Render Target View ) �� ���� ������ �� �� RTV ����. ������ 145 ~ 148
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

	// DirectX �� �׷��� ȭ�� ũ�⸦ �����Ѵ�.
	m_clsViewPort = CD3DX12_VIEWPORT( 0.0f, 0, (float)iWidth, (float)iHeight );
	m_clsViewRect = CD3DX12_RECT( 0, 0, iWidth, iHeight );

	// ��Ʈ ���� : �׸��� ȣ�� ���� ���� ���α׷��� �ݵ�� ������ ���������ο� ����� �ϴ� �ڿ����� �����̰� �� �ڿ����� ���̴� �Է� �������͵鿡 ��� �����Ǵ����� �����Ѵ�.
	CD3DX12_ROOT_PARAMETER arrRootParam[1];
	CD3DX12_DESCRIPTOR_RANGE clsDescRange;
	clsDescRange.Init( D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0 );
	arrRootParam[0].InitAsDescriptorTable( 1, &clsDescRange );

	CD3DX12_ROOT_SIGNATURE_DESC clsSigDesc;
	clsSigDesc.Init( 1, arrRootParam, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT );

	Microsoft::WRL::ComPtr<ID3DBlob> pclsSignature;
	Microsoft::WRL::ComPtr<ID3DBlob> pclsError;
	CHECK_FAILED( D3D12SerializeRootSignature( &clsSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pclsSignature, &pclsError ) );
	CHECK_FAILED( m_pclsDevice->CreateRootSignature( 0, pclsSignature->GetBufferPointer(), pclsSignature->GetBufferSize(), IID_PPV_ARGS( &m_pclsRootSignature ) ) );

	return CreateChild();
}

/**
 * @ingroup LibDirectX12
 * @brief DirectX 12 �� �̿��Ͽ��� ȭ�鿡 �׸���.
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
 */
bool CDirectX12::Draw()
{
	// ��� �Ҵ��ڿ� ��� ����� �缳���Ѵ�.
	CHECK_FAILED( m_pclsCmdAlloc->Reset() );
	CHECK_FAILED( m_pclsCmdList->Reset( m_pclsCmdAlloc.Get(), m_pclsPipeLineState.Get() ) );

	m_pclsCmdList->SetGraphicsRootSignature( m_pclsRootSignature.Get() );

	// ����Ʈ ����. ����Ʈ�� ����� �׷� �ְ��� �ϴ� �ĸ� ������ �κ����簢�� ������ �ǹ��Ѵ�. ������ 153 ~ 155
	m_pclsCmdList->RSSetViewports( 1, &m_clsViewPort );

	// ���� ���簢�� ����. �ĸ� ���۸� �������� ���� ���簢���� ����, �����ϸ�, ������ �� ���� ���簢���� �ٱ��� �ִ� �ȼ����� �ĸ� ���ۿ� ������ȭ���� �ʴ´�. ������ 155 ~ 156
	m_pclsCmdList->RSSetScissorRects( 1, &m_clsViewRect );

	// �ڿ� �뵵�� ���õ� ���� ���̸� Direct3D �� �����Ѵ�. GPU �� �ڿ��� �ڷḦ �� ������� ���� ���¿��� �ڿ��� �ڷḦ ���� �ʵ��� �ϱ� ���� ���
	m_pclsCmdList->ResourceBarrier( 1, &CD3DX12_RESOURCE_BARRIER::Transition( m_pclsRtvBuf[m_iRtvBufIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET ) );

	// ������ ����� ��ϵ� ���� ��� ���۸� ������ ��, �ĸ� ���۸� �����. ������ 182
	UINT iRtpDescSize = m_pclsDevice->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_RTV );
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle( m_pclsRtvHeap->GetCPUDescriptorHandleForHeapStart(), m_iRtvBufIndex, iRtpDescSize );
	m_pclsCmdList->OMSetRenderTargets( 1, &rtvHandle, FALSE, nullptr );

	const float clearColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_pclsCmdList->ClearRenderTargetView( rtvHandle, clearColor, 0, nullptr );
	m_pclsCmdList->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	DrawChild();

	// �ڿ� �뵵�� ���õ� ���� ���̸� Direct3D �� �����Ѵ�.
	m_pclsCmdList->ResourceBarrier( 1, &CD3DX12_RESOURCE_BARRIER::Transition( m_pclsRtvBuf[m_iRtvBufIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT ) );
	CHECK_FAILED( m_pclsCmdList->Close() );

	ID3D12CommandList * arrCmdList[] = { m_pclsCmdList.Get() };
	m_pclsCmdQueue->ExecuteCommandLists( _countof( arrCmdList ), arrCmdList );

	// �ĸ� ���ۿ� ���� ���۸� ��ȯ�Ѵ�. ������ 183
	CHECK_FAILED( m_pclsSwapChain->Present( 1, 0 ) );

	WaitCmdQueue();

	++m_iRtvBufIndex;
	if( m_iRtvBufIndex >= SWAP_CHAIN_BUF_COUNT )
	{
		m_iRtvBufIndex = 0;
	}

	return true;
}

/**
 * @ingroup LibDirectX12
 * @brief GPU ����� �Ϸ�� ������ ����Ѵ�. ������ 132 ~ 134
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
