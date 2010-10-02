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

#ifndef HSERVICES_SETUPDATA_H_
#define HSERVICES_SETUPDATA_H_

#include <HUpnpCore/HServiceId>
#include <HUpnpCore/HServiceInfo>
#include <HUpnpCore/HResourceType>

#include <QtCore/QHash>
#include <QtCore/QString>

template<typename T>
class QSet;

namespace Herqq
{

namespace Upnp
{

/*!
 * This class is used to specify information that is required to setup an
 * HService.
 *
 * \headerfile hservices_setupdata.h HServiceSetup
 *
 * \ingroup hupnp_devicemodel
 *
 * \sa HServicesSetupData, HService
 *
 * \remarks this class is not thread-safe.
 */
class H_UPNP_CORE_EXPORT HServiceSetup
{
H_DISABLE_COPY(HServiceSetup)

private:

    HServiceId m_serviceId;
    HResourceType m_serviceType;
    HService* m_service;
    qint32 m_version;
    HInclusionRequirement m_inclusionReq;

public:

    /*!
     * Creates a new, invalid instance.
     *
     * \sa isValid()
     */
    HServiceSetup();

    /*!
     * Creates a new instance.
     *
     * \param id specifies the service ID.
     *
     * \param serviceType specifies the service type.
     *
     * \param incReq specifies the \e inclusion \e requirement of the
     * service.
     *
     * \sa isValid()
     *
     * \remarks the version() is set to 1.
     */
    HServiceSetup(
        const HServiceId& id,
        const HResourceType& serviceType,
        HInclusionRequirement incReq = InclusionMandatory);

    /*!
     * Creates a new instance.
     *
     * \param id specifies the service ID.
     *
     * \param serviceType specifies the service type.
     *
     * \param version specifies the version of the UPnP device, which first
     * specified the service.
     *
     * \param incReq specifies the \e inclusion \e requirement of the
     * service.
     *
     * \sa isValid()
     */
    HServiceSetup(
        const HServiceId& id,
        const HResourceType& serviceType,
        qint32 version,
        HInclusionRequirement incReq = InclusionMandatory);

    /*!
     * Creates a new instance.
     *
     * \param id specifies the service ID.
     *
     * \param serviceType specifies the service type.
     *
     * \param service specifies a pointer to a heap-allocated HService.
     * This instance takes the ownership of the object.
     *
     * \param incReq specifies the \e inclusion \e requirement of the
     * service.
     *
     * \sa isValid()
     *
     * \remarks the version() is set to 1.
     */
    HServiceSetup(
        const HServiceId& id,
        const HResourceType& serviceType,
        HService* service,
        HInclusionRequirement incReq = InclusionMandatory);

    /*!
     * Creates a new instance.
     *
     * \param id specifies the service ID.
     *
     * \param serviceType specifies the service type.
     *
     * \param service specifies a pointer to a heap-allocated HService.
     * This instance takes the ownership of the object.
     *
     * \param version specifies the version of the UPnP device, which first
     * specified the service.
     *
     * \param incReq specifies the \e inclusion \e requirement of the
     * service.
     *
     * \sa isValid()
     */
    HServiceSetup(
        const HServiceId& id,
        const HResourceType& serviceType,
        HService* service,
        qint32 version,
        HInclusionRequirement incReq = InclusionMandatory);

    /*!
     * Destroys the instance.
     *
     * Destroys the instance.
     */
    ~HServiceSetup();

    /*!
     * Returns the <em>inclusion requirement</em>.
     *
     * \return the <em>inclusion requirement</em>.
     *
     * \sa setInclusionRequirement()
     */
    inline HInclusionRequirement inclusionRequirement() const
    {
        return m_inclusionReq;
    }

    /*!
     * Indicates if the object is valid.
     *
     * \param checkLevel specifies whether the validity of the object should be
     * checked strictly according to the UDA specification.
     *
     * \return \e true in case the object is valid, that is, the service ID,
     * service type, version and inclusion requirement are all properly defined
     * in respect to the specified \c checkLevel.
     */
    bool isValid(HValidityCheckLevel checkLevel) const;

    /*!
     * Returns the HService pointer associated with the instance.
     *
     * \return the HService pointer associated with the instance. The ownership
     * of the HService is not transferred to the caller.
     *
     * \sa setService(), takeService()
     */
    inline HService* service() const { return m_service; }

    /*!
     * Returns the service ID.
     *
     * \return the service ID.
     *
     * \sa setServiceId()
     */
    inline const HServiceId& serviceId() const { return m_serviceId; }

    /*!
     * Returns the service type.
     *
     * \return the service type.
     *
     * \sa setServiceType()
     */
    inline const HResourceType& serviceType() const { return m_serviceType; }

    /*!
     * Returns the version of the UPnP device, which first specified the service.
     *
     * \return the version of the UPnP device, which first specified the service.
     *
     * \sa setVersion()
     */
    inline qint32 version() const
    {
        return m_version;
    }

    /*!
     * Sets the the <em>inclusion requirement</em>.
     *
     * \param arg specifies the <em>inclusion requirement</em>.
     *
     * \sa inclusionRequirement()
     */
    inline void setInclusionRequirement(HInclusionRequirement arg)
    {
        m_inclusionReq = arg;
    }

    /*!
     * Sets the service ID.
     *
     * \param arg specifies the service ID.
     *
     * \sa serviceId()
     */
    inline void setServiceId(const HServiceId& arg)
    {
        m_serviceId = arg;
    }

    /*!
     * Sets the service type.
     *
     * \param arg specifies the service type.
     *
     * \sa serviceType()
     */
    inline void setServiceType(const HResourceType& arg)
    {
        m_serviceType = arg;
    }

    /*!
     * Associates an HService pointer with this instance.
     *
     * \param arg specifies the HService pointer to be associated with this
     * instance. The instance takes the ownership of the provided HService.
     *
     * \remarks if the instance already has an HService pointer associated with it,
     * the old HService is first deleted, even if the provided HService is null.
     *
     * \sa service()
     */
    inline void setService(HService* arg)
    {
        m_service = arg;
    }

    /*!
     * Sets the version of the UPnP device, which first specified the service.
     *
     * \param version defines the version of the UPnP device,
     * which first specifies the service.
     *
     * \sa version()
     */
    inline void setVersion(qint32 version)
    {
        m_version = version;
    }

     /*!
     * Returns the HService pointer associated with the instance and passes
     * the ownership of the object to the caller.
     *
     * \return the HService pointer associated with the instance and passes
     * the ownership of the object to the caller.
     *
     * \sa service()
     */
    inline HService* takeService()
    {
        HService* retVal = m_service;
        m_service = 0;
        return retVal;
    }

};

/*!
 * This class is used to specify information that can be used to setup multiple
 * HService instances.
 *
 * \headerfile hservices_setupdata.h HServicesSetupData
 *
 * \ingroup hupnp_devicemodel
 *
 * \remarks this class is not thread-safe.
 */
class H_UPNP_CORE_EXPORT HServicesSetupData
{
H_DISABLE_COPY(HServicesSetupData)

private:

    QHash<HServiceId, HServiceSetup*> m_serviceSetupInfos;

public:

    /*!
     * Creates a new, empty instance.
     *
     * \sa isEmpty()
     */
    HServicesSetupData();

    /*!
     * Destroys the instance.
     *
     * Destroys the instance.
     */
    ~HServicesSetupData();

    /*!
     * Retrieves a service setup object.
     *
     * \param id specifies the service ID of the item.
     *
     * \return the item with the specified service ID. A null pointer is returned
     * in case no item with the specified service ID was found.
     *
     * \remarks the ownership of the object is \b not transferred.
     *
     * \sa take(), contains()
     */
    HServiceSetup* get(const HServiceId& id) const;

    /*!
     * Indicates if the instance contains a service setup item that has the
     * specified service ID.
     *
     * \param id specifies the service ID of the item.
     *
     * \return \e true when the instance contains an item with the specified
     * service ID.
     *
     * \sa get()
     */
    bool contains(const HServiceId& id) const;

    /*!
     * Indicates if the object is empty.
     *
     * \return \e true in case the instance has no items.
     */
    bool isEmpty() const;

    /*!
     * Returns the number of contained items.
     *
     * \return the number of contained items.
     */
    qint32 size() const;

    /*!
     * Returns the service IDs of the contained items.
     *
     * \return the service IDs of the contained items.
     */
    QSet<HServiceId> serviceIds() const;

    /*!
     * Inserts a new item.
     *
     * \param newItem specifies the item to be added.
     *
     * \return \e true in case the item was added. The \c newItem will not be added
     * if the instance already contains an item that has the
     * same HDeviceSetup::deviceType() as the \c newItem or the \c newItem is null.
     *
     * \remarks
     * \li The \c newItem has to be heap-allocated and
     * \li the instance takes the ownership of the \c newItem, even if it is not
     * added. If the item is not added the item is deleted.
     */
    bool insert(HServiceSetup* newItem);

    /*!
     * Removes an existing item.
     *
     * \param id specifies the service ID of the item to be removed.
     *
     * \return \e true in case the item was found and removed.
     */
    bool remove(const HServiceId& id);

    /*!
     * Associates an HService pointer with an item.
     *
     * \param id specifies the service ID of the item.
     *
     * \param service specifies a heap-allocated pointer to an HService to be
     * associated with the item. Note that the item takes
     * the ownership of the HService.
     *
     * \return \e true when an item with the specified service ID was found
     * and the provided HService pointer was associated with it.
     *
     * \remarks if an item with the specified ID exists and it already has
     * an HService pointer associated with it, the existing HService is deleted.
     */
    bool setService(const HServiceId& id, HService* service);

    /*!
     * Retrieves a service setup object and removes it from the instance.
     *
     * \param id specifies the service ID of the item.
     *
     * \return the item with the specified service ID. A null pointer is returned
     * in case no item with the specified service ID was found.
     *
     * \remarks the ownership of the object \b is transferred to the caller.
     *
     * \sa get()
     */
    HServiceSetup* take(const HServiceId& id);
};

}
}

#endif /* HSERVICES_SETUPDATA_H_ */