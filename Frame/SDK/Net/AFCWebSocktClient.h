/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) AFHttpEntity ArkGame authors.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/

#pragma once

#include "AFINet.h"
#include "SDK/Core/Base/AFQueue.h"
#include "SDK/Core/Base/AFRWLock.hpp"
#include <brynet/net/SocketLibFunction.h>
#include <brynet/net/WrapTCPService.h>
#include <brynet/net/Connector.h>

#pragma pack(push, 1)

class AFCWebSocktClient : public AFINet
{
public:
    AFCWebSocktClient(brynet::net::WrapTcpService::PTR server = nullptr, brynet::net::AsyncConnector::PTR connector = nullptr)
    {
        mnServerID = 0;
        SetWorking(false);
        nReceiverSize = 0;
        nSendSize = 0;

        if(server)
        {
            m_pServer = server;
        }
        else
        {
            m_pServer = std::make_shared<brynet::net::WrapTcpService>();
        }
        if(connector)
        {
            m_pConector = connector;
        }
        else
        {
            m_pConector = brynet::net::AsyncConnector::Create();
        }
    }

    template<typename BaseType>
    AFCWebSocktClient(BaseType* pBaseType, void (BaseType::*handleRecieve)(const AFIMsgHead& xHead, const int, const char*, const size_t, const AFGUID&), void (BaseType::*handleEvent)(const NetEventType, const AFGUID&, const int))
    {
        mRecvCB = std::bind(handleRecieve, pBaseType, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
        mEventCB = std::bind(handleEvent, pBaseType, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        mnServerID = 0;
        nReceiverSize = 0;
        nSendSize = 0;
        m_pServer = std::make_shared<brynet::net::WrapTcpService>();
        m_pConector = brynet::net::AsyncConnector::Create();
    }

    virtual ~AFCWebSocktClient()
    {
        Final();
    };

public:
    virtual void Update();
    virtual void Start(const std::string& strAddrPort, const int nServerID);
    virtual bool Final() final;
    virtual bool SendMsgWithOutHead(const uint16_t nMsgID, const char* msg, const size_t nLen, const AFGUID& xClientID = 0, const AFGUID& xPlayerID = 0);

    virtual bool CloseNetEntity(const AFGUID& xClient);

    virtual bool IsServer();
    virtual bool Log(int severity, const char* msg);

public:
    void OnWebSockMessageCallBack(const brynet::net::HttpSession::PTR & httpSession,
                                  brynet::net::WebSocketFormat::WebSocketFrameType opcode,
                                  const std::string & payload);
    void OnHttpDisConnection(const brynet::net::HttpSession::PTR & httpSession);
    void OnHttpConnect(const brynet::net::HttpSession::PTR& httpSession);

private:
    bool SendMsg(const char* msg, const size_t nLen, const AFGUID& xClient = 0);

    bool DismantleNet(AFHttpEntity* pEntity);
    void ProcessMsgLogicThread();
    void ProcessMsgLogicThread(AFHttpEntity* pEntity);
    bool CloseSocketAll();

    static void log_cb(int severity, const char* msg);

protected:
    int DeCode(const char* strData, const size_t len, AFCMsgHead& xHead);
    int EnCode(const AFCMsgHead& xHead, const char* strData, const size_t len, std::string& strOutData);

private:
    std::unique_ptr<AFHttpEntity> m_pClientEntity;
    std::string mstrIPPort;
    int mnServerID;
    NET_RECEIVE_FUNCTOR mRecvCB;
    NET_EVENT_FUNCTOR mEventCB;
    AFCReaderWriterLock mRWLock;

    brynet::net::WrapTcpService::PTR m_pServer;
    brynet::net::AsyncConnector::PTR m_pConector;
    brynet::net::TCPSession::PTR m_Session;
};

#pragma pack(pop)