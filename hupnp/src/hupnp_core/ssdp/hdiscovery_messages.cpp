/*
 *  Copyright (C) 2010 Tuomo Penttinen, all rights reserved.
 *
 *  Author: Tuomo Penttinen <tp@herqq.org>
 *
 *  This file is part of Herqq UPnP (HUPnP) library.
 *
 *  Herqq UPnP is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Herqq UPnP is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Herqq UPnP. If not, see <http://www.gnu.org/licenses/>.
 */

#include "hdiscovery_messages.h"

#include "./../dataelements/hresourcetype.h"
#include "./../dataelements/hdiscoverytype.h"
#include "./../dataelements/hproduct_tokens.h"

#include "./../socket/hendpoint.h"
#include "./../../utils/hlogger_p.h"

#include <QHostAddress>
#include <QMetaType>
#include <QDateTime>
#include <QString>
#include <QUrl>

static bool registerMetaTypes()
{
    qRegisterMetaType<Herqq::Upnp::HResourceAvailable>("Herqq::Upnp::HResourceAvailable");
    qRegisterMetaType<Herqq::Upnp::HResourceUnavailable>("Herqq::Upnp::HResourceUnavailable");
    qRegisterMetaType<Herqq::Upnp::HDiscoveryRequest>("Herqq::Upnp::HDiscoveryRequest");
    qRegisterMetaType<Herqq::Upnp::HResourceUpdate>("Herqq::Upnp::HResourceUpdate");
    qRegisterMetaType<Herqq::Upnp::HDiscoveryResponse>("Herqq::Upnp::HDiscoveryResponse");

    return true;
}

static bool test = registerMetaTypes();

namespace Herqq
{

namespace Upnp
{

/*******************************************************************************
 * HResourceAvailablePrivate
 ******************************************************************************/
class HResourceAvailablePrivate
{
public: // attributes

    HProductTokens m_serverTokens;
    HDiscoveryType m_usn;
    QUrl   m_location;
    qint32 m_cacheControlMaxAge;
    qint32 m_bootId;
    qint32 m_configId;
    qint32 m_searchPort;

public: // methods

    HResourceAvailablePrivate();
    ~HResourceAvailablePrivate();
};

HResourceAvailablePrivate::HResourceAvailablePrivate() :
    m_serverTokens(), m_usn(), m_location(), m_cacheControlMaxAge(0),
    m_bootId(0), m_configId(0), m_searchPort(0)
{
}

HResourceAvailablePrivate::~HResourceAvailablePrivate()
{
}

/*******************************************************************************
 * HResourceAvailable
 ******************************************************************************/
HResourceAvailable::HResourceAvailable() :
    h_ptr(new HResourceAvailablePrivate())
{
}

HResourceAvailable::HResourceAvailable(
    quint32 cacheControlMaxAge, const QUrl& location,
    const HProductTokens& serverTokens, const HDiscoveryType& usn,
    qint32 bootId, qint32 configId, qint32 searchPort) :
        h_ptr(new HResourceAvailablePrivate())
{
    HLOG(H_AT, H_FUN);

    if (cacheControlMaxAge < 5)
    {
        cacheControlMaxAge = 5;
    }
    else if (cacheControlMaxAge > 60 * 60 * 24)
    {
        cacheControlMaxAge = 60* 60 * 24;
    }

    if (usn.type() == HDiscoveryType::Undefined)
    {
        HLOG_WARN("USN is not defined");
        return;
    }

    if (!location.isValid() || location.isEmpty())
    {
        HLOG_WARN(QString("Location is not defined"));
        return;
    }

    if (!serverTokens.isValid())
    {
        HLOG_WARN_NONSTD(QString("Server tokens are not defined"));
        // although mandatory according to UDA, some UPnP software
        // do not define this ==> cannot require it
    }

    if (serverTokens.upnpToken().minorVersion() > 0)
    {
        if (bootId < 0 || configId < 0)
        {
            HLOG_WARN("bootId and configId must both be >= 0.");
            return;
        }
        if (searchPort < 49152 || searchPort > 65535)
        {
            searchPort = -1;
        }
    }
    else
    {
        searchPort = -1;
    }

    h_ptr->m_serverTokens       = serverTokens;
    h_ptr->m_usn                = usn;
    h_ptr->m_location           = location;
    h_ptr->m_cacheControlMaxAge = cacheControlMaxAge;
    h_ptr->m_configId           = configId;
    h_ptr->m_bootId             = bootId;
    h_ptr->m_searchPort         = searchPort;
}

HResourceAvailable::HResourceAvailable(const HResourceAvailable& other) :
    h_ptr(new HResourceAvailablePrivate(*other.h_ptr))
{
}

HResourceAvailable& HResourceAvailable::operator=(const HResourceAvailable& other)
{
    HResourceAvailablePrivate* newDptr =
        new HResourceAvailablePrivate(*other.h_ptr);

    delete h_ptr;
    h_ptr = newDptr;

    return *this;
}

HResourceAvailable::~HResourceAvailable()
{
    delete h_ptr;
}

bool HResourceAvailable::isValid(bool strict) const
{
    return (h_ptr->m_usn.type() != HDiscoveryType::Undefined) &&
           (strict ? h_ptr->m_serverTokens.isValid() : true);
}

HProductTokens HResourceAvailable::serverTokens() const
{
    return h_ptr->m_serverTokens;
}

HDiscoveryType HResourceAvailable::usn() const
{
    return h_ptr->m_usn;
}

QUrl HResourceAvailable::location() const
{
    return h_ptr->m_location;
}

qint32 HResourceAvailable::cacheControlMaxAge() const
{
    return h_ptr->m_cacheControlMaxAge;
}

qint32 HResourceAvailable::bootId() const
{
    return h_ptr->m_bootId;
}

qint32 HResourceAvailable::configId() const
{
    return h_ptr->m_configId;
}

qint32 HResourceAvailable::searchPort() const
{
    return h_ptr->m_searchPort;
}

bool operator==(const HResourceAvailable& obj1, const HResourceAvailable& obj2)
{
    return obj1.h_ptr->m_serverTokens == obj2.h_ptr->m_serverTokens &&
           obj1.h_ptr->m_usn          == obj2.h_ptr->m_usn &&
           obj1.h_ptr->m_location     == obj2.h_ptr->m_location &&
           obj1.h_ptr->m_cacheControlMaxAge == obj2.h_ptr->m_cacheControlMaxAge &&
           obj1.h_ptr->m_bootId       == obj2.h_ptr->m_bootId &&
           obj1.h_ptr->m_configId     == obj2.h_ptr->m_configId &&
           obj1.h_ptr->m_searchPort   == obj2.h_ptr->m_searchPort;
}

bool operator!=(const HResourceAvailable& obj1, const HResourceAvailable& obj2)
{
    return !(obj1 == obj2);
}

/*******************************************************************************
 * HResourceUnavailablePrivate
 ******************************************************************************/
class HResourceUnavailablePrivate
{
public: // attributes

    HDiscoveryType m_usn;
    qint32 m_bootId;
    qint32 m_configId;
    HEndpoint m_sourceLocation;

public: // methods

    HResourceUnavailablePrivate();
    ~HResourceUnavailablePrivate();
};

HResourceUnavailablePrivate::HResourceUnavailablePrivate() :
    m_usn(), m_bootId(0), m_configId(0), m_sourceLocation(0)
{
}

HResourceUnavailablePrivate::~HResourceUnavailablePrivate()
{
}

/*******************************************************************************
 * HResourceUnavailable
 ******************************************************************************/
HResourceUnavailable::HResourceUnavailable() :
    h_ptr(new HResourceUnavailablePrivate())
{
}

HResourceUnavailable::HResourceUnavailable(
    const HDiscoveryType& usn, const HEndpoint& sourceLocation,
    qint32 bootId, qint32 configId) :
        h_ptr(new HResourceUnavailablePrivate())
{
    HLOG(H_AT, H_FUN);

    if (usn.type() == HDiscoveryType::Undefined)
    {
        HLOG_WARN("USN is not defined");
        return;
    }

    if ((bootId < 0 && configId >= 0) || (configId < 0 && bootId >= 0))
    {
        HLOG_WARN("If either bootId or configId is specified they both must be >= 0");
        return;
    }

    if (bootId < 0)
    {
        bootId = -1; configId = -1;
    }

    h_ptr->m_usn            = usn;
    h_ptr->m_configId       = configId;
    h_ptr->m_bootId         = bootId;
    h_ptr->m_sourceLocation = sourceLocation;
}

HResourceUnavailable::HResourceUnavailable(const HResourceUnavailable& other) :
    h_ptr(new HResourceUnavailablePrivate(*other.h_ptr))
{
}

HResourceUnavailable& HResourceUnavailable::operator=(
    const HResourceUnavailable& other)
{
    HResourceUnavailablePrivate* newDptr =
        new HResourceUnavailablePrivate(*other.h_ptr);

    delete h_ptr;
    h_ptr = newDptr;

    return *this;
}

HResourceUnavailable::~HResourceUnavailable()
{
    delete h_ptr;
}

HEndpoint HResourceUnavailable::location() const
{
    return h_ptr->m_sourceLocation;
}

bool HResourceUnavailable::isValid(bool strict) const
{
    Q_UNUSED(strict)
    return h_ptr->m_usn.type() != HDiscoveryType::Undefined;
    // if the object is valid, the USN is valid
}

HDiscoveryType HResourceUnavailable::usn() const
{
    return h_ptr->m_usn;
}

qint32 HResourceUnavailable::bootId() const
{
    return h_ptr->m_bootId;
}

qint32 HResourceUnavailable::configId() const
{
    return h_ptr->m_configId;
}

bool operator==(const HResourceUnavailable& obj1, const HResourceUnavailable& obj2)
{
    return obj1.h_ptr->m_usn            == obj2.h_ptr->m_usn &&
           obj1.h_ptr->m_sourceLocation == obj2.h_ptr->m_sourceLocation &&
           obj1.h_ptr->m_bootId         == obj2.h_ptr->m_bootId &&
           obj1.h_ptr->m_configId       == obj2.h_ptr->m_configId;
}

bool operator!=(const HResourceUnavailable& obj1, const HResourceUnavailable& obj2)
{
    return !(obj1 == obj2);
}

/*******************************************************************************
 * HResourceUpdatePrivate
 ******************************************************************************/
class HResourceUpdatePrivate
{
public: // attributes

    HDiscoveryType m_usn;
    QUrl   m_location;
    qint32 m_bootId;
    qint32 m_configId;
    qint32 m_nextBootId;
    qint32 m_searchPort;

public: // methods

    HResourceUpdatePrivate();
    ~HResourceUpdatePrivate();
};

HResourceUpdatePrivate::HResourceUpdatePrivate() :
    m_usn(), m_location(), m_bootId(0), m_configId(0), m_nextBootId(0),
    m_searchPort(0)
{
}

HResourceUpdatePrivate::~HResourceUpdatePrivate()
{
}

/*******************************************************************************
 * HResourceUpdate
 ******************************************************************************/
HResourceUpdate::HResourceUpdate() :
    h_ptr(new HResourceUpdatePrivate())
{
}

HResourceUpdate::HResourceUpdate(
    const QUrl& location, const HDiscoveryType& usn,
    qint32 bootId, qint32 configId, qint32 nextBootId, qint32 searchPort) :
        h_ptr(new HResourceUpdatePrivate())
{
    HLOG(H_AT, H_FUN);

    if (usn.type() == HDiscoveryType::Undefined)
    {
        HLOG_WARN("USN is not defined");
        return;
    }

    if (!location.isValid())
    {
        HLOG_WARN("Location is not defined");
        return;
    }

    if ((bootId     < 0 && (configId >= 0 || nextBootId >= 0)) ||
        (configId   < 0 && (bootId   >= 0 || nextBootId >= 0)) ||
        (nextBootId < 0 && (bootId   >= 0 || configId   >= 0)))
    {
        HLOG_WARN("If bootId, configId or nextBootId is specified, " \
                  "they all must be >= 0.");
        return;
    }

    if (bootId < 0)
    {
        bootId = -1; configId = -1; nextBootId = -1; searchPort = -1;
    }
    else if (searchPort < 49152 || searchPort > 65535)
    {
        searchPort = -1;
    }

    h_ptr->m_usn        = usn;
    h_ptr->m_location   = location;
    h_ptr->m_configId   = configId;
    h_ptr->m_bootId     = bootId;
    h_ptr->m_nextBootId = nextBootId;
    h_ptr->m_searchPort = searchPort;
}

HResourceUpdate::HResourceUpdate(const HResourceUpdate& other) :
    h_ptr(new HResourceUpdatePrivate(*other.h_ptr))
{
}

HResourceUpdate& HResourceUpdate::operator=(const HResourceUpdate& other)
{
    HResourceUpdatePrivate* newDptr = new HResourceUpdatePrivate(*other.h_ptr);

    delete h_ptr;
    h_ptr = newDptr;
    return *this;
}

HResourceUpdate::~HResourceUpdate()
{
    delete h_ptr;
}

bool HResourceUpdate::isValid(bool strict) const
{
    Q_UNUSED(strict)
    return h_ptr->m_usn.type() != HDiscoveryType::Undefined;
    // if the object is valid, the USN is valid
}

HDiscoveryType HResourceUpdate::usn() const
{
    return h_ptr->m_usn;
}

QUrl HResourceUpdate::location() const
{
    return h_ptr->m_location;
}

qint32 HResourceUpdate::bootId() const
{
    return h_ptr->m_bootId;
}

qint32 HResourceUpdate::configId() const
{
    return h_ptr->m_configId;
}

qint32 HResourceUpdate::nextBootId() const
{
    return h_ptr->m_nextBootId;
}

qint32 HResourceUpdate::searchPort() const
{
    return h_ptr->m_searchPort;
}

bool operator==(const HResourceUpdate& obj1, const HResourceUpdate& obj2)
{
    return obj1.h_ptr->m_usn        == obj2.h_ptr->m_usn &&
           obj1.h_ptr->m_location   == obj2.h_ptr->m_location &&
           obj1.h_ptr->m_bootId     == obj2.h_ptr->m_bootId &&
           obj1.h_ptr->m_configId   == obj2.h_ptr->m_configId &&
           obj1.h_ptr->m_searchPort == obj2.h_ptr->m_searchPort;
}

bool operator!=(const HResourceUpdate& obj1, const HResourceUpdate& obj2)
{
    return !(obj1 == obj2);
}

/*******************************************************************************
 * HDiscoveryRequestPrivate
 ******************************************************************************/
class HDiscoveryRequestPrivate
{
public: // attributes

    HDiscoveryType m_st;
    qint32    m_mx;
    HProductTokens m_userAgent;

public: // methods

    HDiscoveryRequestPrivate() : m_st(), m_mx(0), m_userAgent() {}

    bool init(const HDiscoveryType& st, qint32 mx, const HProductTokens& userAgent)
    {
        HLOG(H_AT, H_FUN);

        if (st.type() == HDiscoveryType::Undefined)
        {
            HLOG_WARN("Search Target is not specified");
            return false;
        }

        if (mx < 1)
        {
            HLOG_WARN("MX cannot be smaller than 1.");
            return false;
        }
        else if (mx > 5)
        {
            HLOG_WARN("MX is larger than 5, setting it to 5.");
            mx = 5; // UDA instructs to treat MX values larger than 5 as 5
        }

        if (!userAgent.isValid())
        {
            HLOG_WARN_NONSTD(QString("Invalid user agent: [%1]").arg(
                userAgent.toString()));
        }

        m_st        = st;
        m_mx        = mx;
        m_userAgent = userAgent;

        return true;
    }
};

/*******************************************************************************
 * HDiscoveryRequest
 ******************************************************************************/
HDiscoveryRequest::HDiscoveryRequest() :
    h_ptr(new HDiscoveryRequestPrivate())
{
}

HDiscoveryRequest::HDiscoveryRequest(
    qint32 mx, const HDiscoveryType& st, const HProductTokens& userAgent) :
        h_ptr(new HDiscoveryRequestPrivate())
{
    h_ptr->init(st, mx, userAgent);
}

HDiscoveryRequest::HDiscoveryRequest(const HDiscoveryRequest& other) :
    h_ptr(new HDiscoveryRequestPrivate(*other.h_ptr))
{
}

HDiscoveryRequest& HDiscoveryRequest::operator=(const HDiscoveryRequest& other)
{
    HDiscoveryRequestPrivate* newDptr =
        new HDiscoveryRequestPrivate(*other.h_ptr);

    delete h_ptr;
    h_ptr = newDptr;

    return *this;
}

HDiscoveryRequest::~HDiscoveryRequest()
{
    delete h_ptr;
}

bool HDiscoveryRequest::isValid(bool strict) const
{
    return (h_ptr->m_st.type() != HDiscoveryType::Undefined) &&
           (strict ? h_ptr->m_userAgent.isValid() : true);
}

HDiscoveryType HDiscoveryRequest::searchTarget() const
{
    return h_ptr->m_st;
}

qint32 HDiscoveryRequest::mx() const
{
    return h_ptr->m_mx;
}

HProductTokens HDiscoveryRequest::userAgent() const
{
    return h_ptr->m_userAgent;
}

bool operator==(const HDiscoveryRequest& obj1, const HDiscoveryRequest& obj2)
{
    return obj1.h_ptr->m_mx        == obj2.h_ptr->m_mx &&
           obj1.h_ptr->m_st        == obj2.h_ptr->m_st &&
           obj1.h_ptr->m_userAgent == obj2.h_ptr->m_userAgent;
}

bool operator!=(const HDiscoveryRequest& obj1, const HDiscoveryRequest& obj2)
{
    return !(obj1 == obj2);
}

/*******************************************************************************
 * HDiscoveryResponsePrivate
 ******************************************************************************/
class HDiscoveryResponsePrivate
{
public: // attributes

    HProductTokens m_serverTokens;
    HDiscoveryType m_usn;
    QUrl      m_location;
    QDateTime m_date;
    qint32    m_cacheControlMaxAge;
    qint32    m_bootId;
    qint32    m_configId;
    qint32    m_searchPort;

public: // methods

    HDiscoveryResponsePrivate() :
        m_serverTokens(), m_usn(), m_location(), m_date(),
        m_cacheControlMaxAge(0), m_bootId(0), m_configId(0), m_searchPort(0)
    {
    }

};

/*******************************************************************************
 * HDiscoveryResponse
 ******************************************************************************/
HDiscoveryResponse::HDiscoveryResponse() :
    h_ptr(new HDiscoveryResponsePrivate())
{
}

HDiscoveryResponse::HDiscoveryResponse(
    quint32 cacheControlMaxAge, const QDateTime& /*date*/, const QUrl& location,
    const HProductTokens& serverTokens, const HDiscoveryType& usn,
    qint32 bootId, qint32 configId, qint32 searchPort) :
        h_ptr(new HDiscoveryResponsePrivate())
{
    HLOG(H_AT, H_FUN);

    if (cacheControlMaxAge < 5)
    {
        cacheControlMaxAge = 5;
    }
    else if (cacheControlMaxAge > 60 * 60 * 24)
    {
        cacheControlMaxAge = 60* 60 * 24;
    }

    if (usn.type() == HDiscoveryType::Undefined)
    {
        HLOG_WARN("USN is not defined");
        return;
    }

    if (!location.isValid())
    {
        HLOG_WARN("Invalid resource location");
        return;
    }

    if (!serverTokens.isValid())
    {
        HLOG_WARN_NONSTD(QString("Invalid server tokens: %1").arg(
            serverTokens.toString()));
    }

    if (serverTokens.upnpToken().minorVersion() > 0)
    {
        if (bootId < 0 || configId < 0)
        {
            HLOG_WARN("bootId and configId must both be positive.");
            return;
        }
    }

    h_ptr->m_serverTokens       = serverTokens;
    h_ptr->m_usn                = usn;
    h_ptr->m_location           = location;
    h_ptr->m_date               = QDateTime::currentDateTime();
    h_ptr->m_cacheControlMaxAge = cacheControlMaxAge;
    h_ptr->m_bootId             = bootId;
    h_ptr->m_configId           = configId;
    h_ptr->m_searchPort         = searchPort;
}

HDiscoveryResponse::HDiscoveryResponse(const HDiscoveryResponse& other) :
    h_ptr(new HDiscoveryResponsePrivate(*other.h_ptr))
{
}

HDiscoveryResponse& HDiscoveryResponse::operator=(const HDiscoveryResponse& other)
{
    HDiscoveryResponsePrivate* newDptr =
        new HDiscoveryResponsePrivate(*other.h_ptr);

    delete h_ptr;
    h_ptr = newDptr;

    return *this;
}

HDiscoveryResponse::~HDiscoveryResponse()
{
    delete h_ptr;
}

bool HDiscoveryResponse::isValid(bool strict) const
{
    return (h_ptr->m_usn.type() != HDiscoveryType::Undefined) &&
           (strict ? h_ptr->m_serverTokens.isValid() : true);
}

HProductTokens HDiscoveryResponse::serverTokens() const
{
    return h_ptr->m_serverTokens;
}

QDateTime HDiscoveryResponse::date() const
{
    return h_ptr->m_date;
}

HDiscoveryType HDiscoveryResponse::usn() const
{
    return h_ptr->m_usn;
}

QUrl HDiscoveryResponse::location() const
{
    return h_ptr->m_location;
}

qint32 HDiscoveryResponse::cacheControlMaxAge() const
{
    return h_ptr->m_cacheControlMaxAge;
}

qint32 HDiscoveryResponse::bootId() const
{
    return h_ptr->m_bootId;
}

qint32 HDiscoveryResponse::configId() const
{
    return h_ptr->m_configId;
}

qint32 HDiscoveryResponse::searchPort() const
{
    return h_ptr->m_searchPort;
}

bool operator==(const HDiscoveryResponse& obj1, const HDiscoveryResponse& obj2)
{
    return obj1.h_ptr->m_serverTokens == obj2.h_ptr->m_serverTokens &&
           obj1.h_ptr->m_usn          == obj2.h_ptr->m_usn &&
           obj1.h_ptr->m_location     == obj2.h_ptr->m_location &&
           obj1.h_ptr->m_cacheControlMaxAge == obj2.h_ptr->m_cacheControlMaxAge &&
           obj1.h_ptr->m_bootId       == obj2.h_ptr->m_bootId &&
           obj1.h_ptr->m_configId     == obj2.h_ptr->m_configId &&
           obj1.h_ptr->m_searchPort   == obj2.h_ptr->m_searchPort;// &&
           //obj1.h_ptr->m_date         == obj2.h_ptr->m_date;
    // the date isn't used in comparison due to something that seems to be a bug
    // in Qt. A datetime object created using the currentDateTime() function
    // copy constructed to another datetime object seems to result a logically
    // different object.
}

bool operator!=(const HDiscoveryResponse& obj1, const HDiscoveryResponse& obj2)
{
    return !(obj1 == obj2);
}

}
}
