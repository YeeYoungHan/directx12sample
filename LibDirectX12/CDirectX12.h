#pragma once

#include <windows.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <string>
#include "d3dx12.h"

#define SWAP_CHAIN_BUF_COUNT	2

class CDirectX12
{
public:
	CDirectX12();
	virtual ~CDirectX12();

	bool Create( HWND hWnd );
	bool WaitCmdQueue();

	const TCHAR * GetErrString();

protected:
	Microsoft::WRL::ComPtr<IDXGIFactory4> m_pclsFactory;
	Microsoft::WRL::ComPtr<ID3D12Device> m_pclsDevice;

	// CPU, GPU ����ȭ
	Microsoft::WRL::ComPtr<ID3D12Fence> m_pclsFence;
	UINT64 m_iFence;

	// ��� ��⿭, �޸� �Ҵ���, ��� ���
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_pclsCmdQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_pclsCmdAlloc;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_pclsCmdList;

	// ��ȯ �罽
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_pclsSwapChain;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_pclsRtvHeap;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_pclsRtvBuf[SWAP_CHAIN_BUF_COUNT];

	int m_iRtvBufIndex;

	// ���� ����
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
