/*
 *  Copyright (C) 2010 Tuomo Penttinen, all rights reserved.
 *
 *  Author: Tuomo Penttinen <tp@herqq.org>
 *
 *  This file is part of Herqq UPnP (HUPnP) library.
 *
 *  Herqq UPnP is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Herqq UPnP is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with Herqq UPnP. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef HACTIONINVOKE_PROXY_P_H_
#define HACTIONINVOKE_PROXY_P_H_

//
// !! Warning !!
//
// This file is not part of public API and it should
// never be included in client code. The contents of this file may
// change or the file may be removed without of notice.
//

#include "../../general/hupnp_fwd.h"
#include "../../http/hhttp_asynchandler_p.h"

#include "../../devicemodel/haction_p.h"
#include "../../devicemodel/hactionarguments.h"

#include <QUrl>
#include <QMutex>
#include <QQueue>
#include <QString>
#include <QTcpSocket>
#include <QScopedPointer>

namespace Herqq
{

namespace Upnp
{

class HActionInvokeProxyImpl;

//
// Class for relaying action invocations across the network to the real
// HAction objects instantiated by device hosts
//
class HActionProxy :
    public QObject
{
Q_OBJECT
H_DISABLE_COPY(HActionProxy)

private:

    HService* m_service;
    // the service to which the action belongs. the service is required
    // for fetching certain information the action invocation requires

    QString m_actionName;
    // the name of the *real* action we are trying to invoke (at the device side)

    HActionArguments m_inArgs;
    // used to validate the user-provided arguments going to the real action

    HActionArguments m_outArgs;
    // used to validate the arguments coming from the real action.

    QScopedPointer<HHttpAsyncHandler> m_http;
    // object that enables easier asynchronous invocation over http using
    // the event loop

    QScopedPointer<QTcpSocket> m_sock;
    // persistent socket for communication

    const QByteArray m_loggingIdentifier;

    QList<QUrl> m_locations;
    qint32 m_iNextLocationToTry;
    // the device locations and the index the next connection attempt should try
    // these are the places to which the action invocation requests are sent

    HAsyncInvocation* m_invocationInProgress;

    MessagingInfo m_messagingInfo;

    HActionInvokeProxyImpl* m_owner;

private:

    bool connectToHost();
    void invocationDone(qint32 rc);

private slots:

    void invoke_slot();

    void error(QAbstractSocket::SocketError);

    void send();
    void msgIoComplete(HHttpAsyncOperation*);

Q_SIGNALS:

    void invoke_sig();

public:

    HActionProxy(
        const QByteArray& loggingIdentifier,
        HAction* action,
        HActionInvokeProxyImpl* owner);

    virtual ~HActionProxy();

    void beginInvoke(HAsyncInvocation*);
};

//
//
//
class HActionInvokeProxyImpl :
    public HActionInvokeProxy
{
H_DISABLE_COPY(HActionInvokeProxyImpl)
friend class HActionProxy;

private:

    void invokeCompleted();

    HActionProxy* m_proxy;
    QQueue<HAsyncInvocation*> m_invocations;
    QMutex m_invocationsMutex;

public:

    HActionInvokeProxyImpl(
        const QByteArray& loggingIdentifier,
        HAction* action, QThread* parentThread);

    virtual ~HActionInvokeProxyImpl();
    virtual bool beginInvoke(HAsyncInvocation*);
};

}
}

#endif /* HACTIONINVOKE_PROXY_P_H_ */
