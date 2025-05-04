// module kkio.windows.init;


#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <cstdio>
#include <mswsock.h>
#include <print>
#include <stacktrace>
#include <format>

namespace kkio::windows {

    namespace {
        bool isWinsockInitialized = false;
        ::LPFN_ACCEPTEX acceptExPtr{};
        ::LPFN_CONNECTEX fnConnectExPtr{};
        ::LPFN_DISCONNECTEX fnDisconnectExPtr{};

        auto create_temp_socket() -> SOCKET {
            SOCKET tempSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
            if (tempSocket == INVALID_SOCKET) {
                auto msg = std::format("Failed to create temporary socket: {}\n at {}",
                    WSAGetLastError(),
                    std::stacktrace::current()
                );
                std::println(stderr, "{}", msg);
                throw std::runtime_error(msg);
            }
            return tempSocket;
        }

        auto load_winsock_extensions() -> void {
            SOCKET tempSocket = create_temp_socket();

            GUID guidAcceptEx = WSAID_ACCEPTEX;
            GUID guidConnectEx = WSAID_CONNECTEX;
            GUID guidDisconnectEx = WSAID_DISCONNECTEX;

            DWORD dwBytes = 0;

            if (WSAIoctl(tempSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
                         &guidAcceptEx, sizeof(guidAcceptEx),
                         &acceptExPtr, sizeof(acceptExPtr),
                         &dwBytes, NULL, NULL) == SOCKET_ERROR) {
                fprintf(stderr, "Failed to load AcceptEx: %d\n", WSAGetLastError());
                         }

            if (WSAIoctl(tempSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
                         &guidConnectEx, sizeof(guidConnectEx),
                         &fnConnectExPtr, sizeof(fnConnectExPtr),
                         &dwBytes, NULL, NULL) == SOCKET_ERROR) {
                fprintf(stderr, "Failed to load ConnectEx: %d\n", WSAGetLastError());
                         }

            if (WSAIoctl(tempSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
                         &guidDisconnectEx, sizeof(guidDisconnectEx),
                         &fnDisconnectExPtr, sizeof(fnDisconnectExPtr),
                         &dwBytes, NULL, NULL) == SOCKET_ERROR) {
                fprintf(stderr, "Failed to load DisconnectEx: %d\n", WSAGetLastError());
                         }

            closesocket(tempSocket);
        }
    }

    auto initWinSock() -> void {
        if (isWinsockInitialized) return;
        WSADATA wsaData{};
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            auto msg = std::format("WSAStartup failed.\n{}", std::stacktrace::current());
            std::println(stderr, "{}", msg);
            throw std::runtime_error(msg);
        }
        load_winsock_extensions();
        isWinsockInitialized = true;
    }

    auto closeWinSock() -> void {
        WSACleanup();
    }

    auto acceptEx(
      _In_ SOCKET sListenSocket,
      _In_ SOCKET sAcceptSocket,
      _Out_writes_bytes_(dwReceiveDataLength+dwLocalAddressLength+dwRemoteAddressLength) PVOID lpOutputBuffer,
      _In_ DWORD dwReceiveDataLength,
      _In_ DWORD dwLocalAddressLength,
      _In_ DWORD dwRemoteAddressLength,
      _Out_ LPDWORD lpdwBytesReceived,
      _Inout_ LPOVERLAPPED lpOverlapped
    ) noexcept -> BOOL {
        return acceptExPtr(sListenSocket, sAcceptSocket, lpOutputBuffer, dwReceiveDataLength,
            dwLocalAddressLength, dwRemoteAddressLength, lpdwBytesReceived, lpOverlapped);
    }

    auto connectEx(
        _In_ SOCKET s,
        _In_reads_bytes_(namelen) const struct sockaddr FAR *name,
        _In_ int namelen,
        _In_reads_bytes_opt_(dwSendDataLength) PVOID lpSendBuffer,
        _In_ DWORD dwSendDataLength,
        _Out_ LPDWORD lpdwBytesSent,
        _Inout_ LPOVERLAPPED lpOverlapped
        ) noexcept -> BOOL {
        return fnConnectExPtr(s, name, namelen, lpSendBuffer, dwSendDataLength,lpdwBytesSent, lpOverlapped);
    }

} // namespace kkio::windows
