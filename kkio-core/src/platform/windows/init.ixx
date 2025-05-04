
module;

import std;

//#include <winsock2.h>
#include <Windows.h>

export module kkio.windows.init;

export namespace kkio::windows {

    auto initWinSock() -> void;

    auto closeWinSock() -> void;

    auto acceptEx(
        _In_ SOCKET sListenSocket,
        _In_ SOCKET sAcceptSocket,
        _Out_writes_bytes_(dwReceiveDataLength+dwLocalAddressLength+dwRemoteAddressLength) PVOID lpOutputBuffer,
        _In_ DWORD dwReceiveDataLength,
        _In_ DWORD dwLocalAddressLength,
        _In_ DWORD dwRemoteAddressLength,
        _Out_ LPDWORD lpdwBytesReceived,
        _Inout_ LPOVERLAPPED lpOverlapped
    ) noexcept -> BOOL;

    auto connectEx(
        _In_ SOCKET s,
        _In_reads_bytes_(namelen) const struct sockaddr FAR *name,
        _In_ int namelen,
        _In_reads_bytes_opt_(dwSendDataLength) PVOID lpSendBuffer,
        _In_ DWORD dwSendDataLength,
        _Out_ LPDWORD lpdwBytesSent,
        _Inout_ LPOVERLAPPED lpOverlapped
    ) noexcept -> BOOL;

} // namespace kkio::windows
