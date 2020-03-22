#include "RemoteGamepadServer.h"
#include <iostream>
#include <thread>

#include <ViGEm/Client.h>

#include <gamepad_state_serializer.h>

namespace
{
    void applyGamepadState(const XUSB_REPORT& gamepadState)
    {

    }
}

RemoteGamepad::Server::Server(unsigned short port)
    : m_socket(m_IOService)
    , m_acceptor(m_IOService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
    , m_vigemClient(vigem_alloc())
    , m_vigemTagret(vigem_target_x360_alloc())
{
    auto result = vigem_connect(m_vigemClient);

    if (result != VIGEM_ERROR_NONE)
    {
        std::cerr << "Failed to connect to the virtual gamepad. Are you sure you have installed the driver? Error code: " << result << '\n';
        throw result;
    }

    result = vigem_target_add(m_vigemClient, m_vigemTagret);

    if (result != VIGEM_ERROR_NONE)
    {
        std::cerr << "Failed to attach virtual gamepad target. Error code: " << result << '\n';
        throw result;
    }
}

void RemoteGamepad::Server::receive()
{
    try
    {
        std::cout << "Waiting for connection...\n";
        m_acceptor.accept(m_socket);
        std::cout << "Client connected: " << m_socket.remote_endpoint().address().to_string() << '\n';

        while (true)
        {
            boost::asio::streambuf buffer;

#ifdef _DEBUG
            boost::asio::read_until(m_socket, buffer, '\n');

            const std::string packedGamepadState(boost::asio::buffer_cast<const char*>(buffer.data()), buffer.size());

            std::cout << "Received: " << std::ios::binary << boost::asio::buffer_cast<const char*>(buffer.data()) << '\n';

            const auto gamepadState = deserializeGamepadState(packedGamepadState);

            applyGamepadState(gamepadState);
#else

#endif
        }
    }
    catch (const boost::system::system_error& error)
    {
        std::cerr << "Connection failure: couldn't accept connection. Error code: " << error.code() << ", description: " << error.what() << '\n';
        return;
    }
}
