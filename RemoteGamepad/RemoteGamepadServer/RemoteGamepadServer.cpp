#include "RemoteGamepadServer.h"
#include <iostream>
#include <thread>

#include <ViGEm/Client.h>

#include <gamepad_state_serializer.h>
#include <logging.h>
#include <Exception.h>


RemoteGamepad::Server::Server(unsigned short port)
try:
      m_socket(m_IOService)
    , m_acceptor(m_IOService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
    , m_vigemClient(vigem_alloc())
    , m_vigemTagret(vigem_target_x360_alloc())
{
    VIGEM_ERROR result = vigem_connect(m_vigemClient);

    if (result != VIGEM_ERROR_NONE)
    {
        throw Exception("Failed to connect virtual gamepad.")
            .withArgument("error code", result);
    }

    result = vigem_target_add(m_vigemClient, m_vigemTagret);

    if (result != VIGEM_ERROR_NONE)
    {
        throw Exception("Failed to attach the virtual gamepad target.")
            .withArgument("error code", result);
    }
}
catch (const boost::system::system_error& error)
{
    throw Exception("Server init error.").withArgument("error text", error.what());
}

RemoteGamepad::Server::~Server()
{
    vigem_target_remove(m_vigemClient, m_vigemTagret);
    vigem_target_free(m_vigemTagret);

    vigem_free(m_vigemClient);
}

void RemoteGamepad::Server::connectWithClient()
{
    boost::system::error_code error;

    Logging::StdOut()->info("Waiting for connection...");
    m_acceptor.accept(m_socket, error);

    if (error.failed())
    {
        throw Exception("Couldn't accept the connection.")
            .withArgument("error code", error.value())
            .withArgument("error message", error.message());
    }

    Logging::StdOut()->info("Client connected: ", m_socket.remote_endpoint().address().to_string());
}

void RemoteGamepad::Server::receiveData()
{
    boost::asio::streambuf buffer;
    boost::system::error_code error;

    const auto bytes = boost::asio::read(m_socket, buffer, boost::asio::transfer_exactly(sizeof(XINPUT_GAMEPAD)), error);

    if (error.failed())
    {
        Logging::StdErr()->error("Socket read operation failed. Error code: {}, error message: {}", error.value(), error.message());
        return;
    }

    const std::string packedGamepadState(boost::asio::buffer_cast<const char*>(buffer.data()), bytes);

    const auto gamepadState = deserializeGamepadState(packedGamepadState);

    applyGamepadState(gamepadState);
}

void RemoteGamepad::Server::applyGamepadState(const XUSB_REPORT& state) const
{
    vigem_target_x360_update(m_vigemClient, m_vigemTagret, state);
}
