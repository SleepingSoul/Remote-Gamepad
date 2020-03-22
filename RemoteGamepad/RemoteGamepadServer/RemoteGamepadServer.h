#pragma once

#include <future>

#include <boost/asio.hpp>

#include <ViGEm/Client.h>

namespace RemoteGamepad
{
    class Server
    {
    public:
        Server(unsigned short port);
        ~Server();

        void receive();

    private:
        void applyGamepadState(const XUSB_REPORT& state) const;

        boost::asio::io_service m_IOService;
        boost::asio::ip::tcp::socket m_socket;
        boost::asio::ip::tcp::acceptor m_acceptor;

        PVIGEM_CLIENT m_vigemClient;
        PVIGEM_TARGET m_vigemTagret;
    };
}