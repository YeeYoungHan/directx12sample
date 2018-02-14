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

#pragma once

#include <windows.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <string>
#include "d3dx12.h"
#include <DirectXMath.h>

#define SWAP_CHAIN_BUF_COUNT	2

class CDirectX12
{
public:
	CDirectX12();
	virtual ~CDirectX12();

	bool Create( HWND hWnd );
	bool Draw();
	bool WaitCmdQueue();

	const TCHAR * GetErrString();

	virtual bool CreateChild() = 0;
	virtual bool DrawChild() = 0;

protected:
	Microsoft::WRL::ComPtr<IDXGIFactory4> m_pclsFactory;
	Microsoft::WRL::ComPtr<ID3D12Device> m_pclsDevice;

	// CPU, GPU 동기화
	Microsoft::WRL::ComPtr<ID3D12Fence> m_pclsFence;
	UINT64 m_iFence;

	// 명령 대기열, 메모리 할당자, 명령 목록
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_pclsCmdQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_pclsCmdAlloc;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_pclsCmdList;

	// 교환 사슬
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_pclsSwapChain;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_pclsRtvHeap;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_pclsRtvBuf[SWAP_CHAIN_BUF_COUNT];

	int m_iRtvBufIndex;

	// 화면 크기
	CD3DX12_VIEWPORT m_clsViewPort;
	CD3DX12_RECT m_clsViewRect;

	// 루트 서명 : 그리기 호출 전에 응용 프로그램이 반드시 렌더링 파이프라인에 묶어야 하는 자원들이 무엇이고 그 자원들이 셰이더 입력 레지스터들에 어떻게 대응되는지를 정의한다.
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_pclsRootSignature;

	// 파이프라인 상태 객체 (PSO) : 렌더링 파이프라인의 상태를 제어하는 대부분의 객체는 PSO 를 이용하여서 지정된다.
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pclsPipeLineState;

	// 에러 정보
	HRESULT m_iErrCode;
	std::wstring m_strErrFunc;
	std::wstring m_strErrFile;
	int m_iErrLine;
};

#define CHECK_FAILED(x)    \
{                          \
	HRESULT hrt = (x);       \
	if( FAILED(hrt) )        \
	{                        \
		WCHAR szBuf[512];      \
		MultiByteToWideChar( CP_ACP, 0, __FILE__, -1, szBuf, 512 ); \
		m_iErrCode = hrt;      \
		m_strErrFunc = L#x;    \
		m_strErrFile = szBuf;  \
		m_iErrLine = __LINE__; \
		return false;          \
	}                        \
}

