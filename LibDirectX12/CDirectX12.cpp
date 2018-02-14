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
 * @brief DirectX12 사용을 위한 변수들을 생성한다.
 * @param hWnd	윈도우 핸들
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool CDirectX12::Create( HWND hWnd )
{
#ifdef _DEBUG
	// 디버그 로그 활성화
	{
		Microsoft::WRL::ComPtr<ID3D12Debug> pclsDebug;
		CHECK_FAILED( D3D12GetDebugInterface( IID_PPV_ARGS( &pclsDebug ) ) );
		pclsDebug->EnableDebugLayer();
	}
#endif

	// 장치 생성 - DXGIFactory, Device, Fence 생성. 페이지 138 ~ 140
	CHECK_FAILED( CreateDXGIFactory1( IID_PPV_ARGS( &m_pclsFactory ) ) );
	CHECK_FAILED( D3D12CreateDevice( nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS( &m_pclsDevice ) ) );
	CHECK_FAILED( m_pclsDevice->CreateFence( 0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( &m_pclsFence ) ) );

	// 명령 대기열, 메모리 할당자, 명령 목록 객체 생성. 페이지 141 ~ 142
	D3D12_COMMAND_QUEUE_DESC clsQueueDesc = {};
	clsQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	clsQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	CHECK_FAILED( m_pclsDevice->CreateCommandQueue( &clsQueueDesc, IID_PPV_ARGS( &m_pclsCmdQueue ) ) );

	CHECK_FAILED( m_pclsDevice->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( &m_pclsCmdAlloc ) ) );
	CHECK_FAILED( m_pclsDevice->CreateCommandList( 0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pclsCmdAlloc.Get(), nullptr, IID_PPV_ARGS( &m_pclsCmdList ) ) );
	m_pclsCmdList->Close();

	// 교환 사슬 객체 생성. 페이지 142 ~ 145
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

	// RTV( Render Target View ) 를 위한 서술자 힙 및 RTV 생성. 페이지 145 ~ 148
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

	// DirectX 로 그려질 화면 크기를 설정한다.
	m_clsViewPort = CD3DX12_VIEWPORT( 0.0f, 0, (float)iWidth, (float)iHeight );
	m_clsViewRect = CD3DX12_RECT( 0, 0, iWidth, iHeight );

	// 루트 서명 : 그리기 호출 전에 응용 프로그램이 반드시 렌더링 파이프라인에 묶어야 하는 자원들이 무엇이고 그 자원들이 셰이더 입력 레지스터들에 어떻게 대응되는지를 정의한다.
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
 * @brief DirectX 12 를 이용하여서 화면에 그린다.
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool CDirectX12::Draw()
{
	// 명령 할당자와 명령 목록을 재설정한다.
	CHECK_FAILED( m_pclsCmdAlloc->Reset() );
	CHECK_FAILED( m_pclsCmdList->Reset( m_pclsCmdAlloc.Get(), m_pclsPipeLineState.Get() ) );

	m_pclsCmdList->SetGraphicsRootSignature( m_pclsRootSignature.Get() );

	// 뷰포트 설정. 뷰포트는 장면을 그려 넣고자 하는 후면 버퍼의 부분직사각형 영역을 의미한다. 페이지 153 ~ 155
	m_pclsCmdList->RSSetViewports( 1, &m_clsViewPort );

	// 가위 직사각형 설정. 후면 버퍼를 기준으로 가위 직사각형을 정의, 설정하면, 렌더링 시 가위 직사각형의 바깥에 있는 픽셀들은 후면 버퍼에 레스터화되지 않는다. 페이지 155 ~ 156
	m_pclsCmdList->RSSetScissorRects( 1, &m_clsViewRect );

	// 자원 용도에 관련된 상태 전이를 Direct3D 에 통지한다. GPU 가 자원에 자료를 다 기록하지 않은 상태에서 자원의 자료를 읽지 않도록 하기 위한 기능
	m_pclsCmdList->ResourceBarrier( 1, &CD3DX12_RESOURCE_BARRIER::Transition( m_pclsRtvBuf[m_iRtvBufIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET ) );

	// 랜더링 결과가 기록될 랜더 대상 버퍼를 지정한 후, 후면 버퍼를 지운다. 페이지 182
	UINT iRtpDescSize = m_pclsDevice->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_RTV );
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle( m_pclsRtvHeap->GetCPUDescriptorHandleForHeapStart(), m_iRtvBufIndex, iRtpDescSize );
	m_pclsCmdList->OMSetRenderTargets( 1, &rtvHandle, FALSE, nullptr );

	const float clearColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_pclsCmdList->ClearRenderTargetView( rtvHandle, clearColor, 0, nullptr );
	m_pclsCmdList->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	DrawChild();

	// 자원 용도에 관련된 상태 전이를 Direct3D 에 통지한다.
	m_pclsCmdList->ResourceBarrier( 1, &CD3DX12_RESOURCE_BARRIER::Transition( m_pclsRtvBuf[m_iRtvBufIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT ) );
	CHECK_FAILED( m_pclsCmdList->Close() );

	ID3D12CommandList * arrCmdList[] = { m_pclsCmdList.Get() };
	m_pclsCmdQueue->ExecuteCommandLists( _countof( arrCmdList ), arrCmdList );

	// 후면 버퍼와 전면 버퍼를 교환한다. 페이지 183
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
 * @brief GPU 명령이 완료될 때까지 대기한다. 페이지 132 ~ 134
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
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
 * @brief DirectX12 함수 사용시 발생한 오류 정보 문자열을 가져온다.
 * @returns DirectX12 함수 사용시 발생한 오류 정보 문자열을 리턴한다.
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
