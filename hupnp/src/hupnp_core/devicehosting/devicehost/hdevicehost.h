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

#ifndef HDEVICEHOST_H_
#define HDEVICEHOST_H_

#include "./../../general/hdefs_p.h"
#include "./../../general/hupnp_fwd.h"

#include <QObject>

namespace Herqq
{

namespace Upnp
{

class HDeviceHostPrivate;

/*!
 * A class for creating and hosting \c %HDevice instances on the network.
 *
 * \headerfile hdevicehost.h HDeviceHost
 *
 * \ingroup devicehosting
 *
 * As the name implies, this is the class in the %Herqq UPnP library
 * used to expose UPnP devices to UPnP control points.
 * The class \e hosts instances of HDevice, which means that the class takes
 * care of all of the UPnP mechanics detaching the HDevice from it. This separation
 * leaves the HDevice to model the UPnP device structure and to focus on the functionality
 * of the specific device type. This is what the HUPnP \ref devicemodel
 * is all about.
 *
 * Hosting a device is simple, assuming you have the necessary device and service
 * descriptions ready and the HUPnP device and service classes implemented.
 * Basically, you only need to:
 *
 * \li instantiate an HDeviceConfiguration for each UPnP device type to be hosted and
 * pass them to the \c %HDeviceHost inside a HDeviceHostConfiguration instance
 * \li instantiate and initialize an \c %HDeviceHost
 * \li make sure a Qt event loop is present in the thread in which the
 * \c %HDeviceHost is run.
 *
 * As an example, consider the following:
 *
 * \code
 *
 * // myclass.h

 * #include <HDeviceHost>
 * #include <QScopedPointer>
 *
 * #include <QObject>
 *
 * class MyClass :
 *     public QObject
 * {
 * Q_OBJECT
 *
 * private:
 *     Herqq::Upnp::HDeviceHost* m_deviceHost;
 *
 * public:
 *     MyClass(QObject* parent = 0);
 * };
 *
 * // myclass.cpp
 *
 * #include "myclass.h"
 * #include "my_hdevice.h" // your code containing the type MyHDevice
 *
 * namespace
 * {
 * class Creator
 * {
 * public:
 *     Herqq::Upnp::HDevice* operator()(const Herqq::Upnp::HDeviceInfo&)
 *     {
 *         return new MyHDevice(); // your class derived from HDevice
 *     }
 * };
 * }
 *
 * MyClass::MyClass(QObject* parent) :
 *     QObject(parent),
 *         m_deviceHost(new Herqq::Upnp::HDeviceHost(this))
 * {
 *     Herqq::Upnp::HDeviceConfiguration deviceConf;
 *     deviceConf.setPathToDeviceDescription("my_hdevice_devicedescription.xml");
 *
 *     Creator deviceCreator;
 *     // you could also use a normal or a member function to create HDevice
 *     // types
 *
 *     deviceConf.setDeviceCreator(deviceCreator);
 *
 *     if (m_deviceHost->init(deviceConf) != Herqq::Upnp::HDeviceHost::Success)
 *     {
 *         // the initialization failed, perhaps you should do something?
 *         return;
 *     }
 *
 *     // the host is running and your device should now be accessible to
 *     // UPnP Control points until the host is destroyed and assuming the current
 *     // thread has an event loop.
 * }
 *
 * \endcode
 *
 * There are a few noteworthy issues in the example above.
 *
 * -# The device host will fail to initialize if your HDeviceConfiguration
 * instance is invalid; for instance, the \e device \e creator is not specified or
 * the path to your UPnP Device Description is invalid. Similarly, if your
 * UPnP Device or UPnP Service description (if your UPnP device has one) is invalid, the device
 * host will fail to initialize. The point is, you should always \b check \b the \b return \b value.
 * -# Your HDevice is accessible only as long as your \c %HDeviceHost
 * is alive. When the device host is destroyed every UPnP device it hosted
 * are destroyed as well.
 * -# \c %HDeviceHost requires an event loop to function.
 * -# \c %HDeviceHost takes in a HDeviceHostConfiguration object, which has a constructor
 * that takes in a HDeviceConfiguration object. This is exploited in the example above,
 * since we are not interested in hosting multiple HDevice instances in the same host and
 * we are not interested in modifying the default behavior of the \c %HDeviceHost.
 *
 * \remarks
 *
 * \li \c %HDeviceHost has thread affinity, which mandates
 * that the \c %HDeviceHost and any object managed by it must be destroyed in the
 * thread in which the \c %HDeviceHost at the time lives.
 * You can use <c>QObject::moveToThread()</c> on the \c %HDeviceHost, which causes
 * the device host and every object managed by it to be moved to the chosen thread.
 * However, you cannot move individual objects managed by \c %HDeviceHost.
 *
 * \li \c %HDeviceHost is the owner of the instances of
 * \c %HDevice it manages. It handles the memory management
 * of every object it has created. In other words, a device host \b never
 * transfers the ownership of the HDevice objects it manages.
 *
 * \li <b>%HDeviceHost always destroys every %HDevice it manages when it is being destroyed</b>.
 *
 * \sa devicehosting, HDevice, HDeviceHostConfiguration, HDeviceConfiguration
 */
class H_UPNP_CORE_EXPORT HDeviceHost :
    public QObject
{
Q_OBJECT
H_DISABLE_COPY(HDeviceHost)
H_DECLARE_PRIVATE(HDeviceHost)

public:

    /*!
     * Specifies return values that some of the methods of the class may return.
     */
    enum ReturnCode
    {
        /*!
         * Return value signifying general failure.
         *
         * This return code is used when
         * an operation could not be successfully completed, but the exact
         * cause for the error could not be determined.
         */
        UndefinedFailure = -1,

        /*!
         *  Return value used to indicate that an operation succeeded.
         */
        Success = 0,

        /*!
         * Return value signifying that the device host is already successfully
         * initialized.
         */
        AlreadyInitialized = 1,

        /*!
         * Return value signifying that the provided host configuration was incorrect.
         */
        InvalidConfiguration = 2,

        /*!
         * Return value signifying that a provided device description document
         * was invalid.
         */
        InvalidDeviceDescription = 3,

        /*!
         * Return value signifying that a provided service description document
         * was invalid.
         */
        InvalidServiceDescription = 4,

        /*!
         * Return value used to indicate one or more more problems in communications
         * layer.
         *
         * For instance, perhaps the HTTP server could not be started or SSDP
         * listener could not be initialized.
         */
        CommunicationsError = 5
    };

private:

    /*!
     * Performs the initialization of a derived class.
     *
     * The \c %HDeviceHost uses two-phase initialization, in which the user
     * first constructs an instance and then calls init() in order to ready
     * the object for use. This method is called by the \c %HDeviceHost
     * during its private initialization after all the private data structures
     * are constructed but before any network activity. At this point, no HTTP
     * or SSDP requests are served.
     *
     * You can override this method to perform any further initialization of a derived
     * class.
     *
     * \return HDeviceHost::Success if and only if the initialization succeeded.
     * If any other value is returned, the initialization of the device host is
     * aborted with the error code returned by the derived class.
     *
     * \remarks the default implementation does nothing.
     *
     * \sa init()
     */
    virtual ReturnCode doInit();

    /*!
     * Performs the de-initialization of a derived class.
     *
     * Since it is possible to shutdown a device host without actually destroying the
     * instance by calling quit(), derived classes have the possibility to
     * perform their own de-initialization procedure by overriding this method.
     * This method is called \b before the \c %HDeviceHost cleans its
     * private data structures but after it has stopped listening requests
     * from the network.
     *
     * \remarks the default implementation does nothing.
     *
     * \sa quit()
     */
    virtual void doQuit();

    /*!
     * Checks if a (re-)subscription should be accepted.
     *
     * Derived classes can opt to override this method to decide what
     * event subscriptions are accepted and what are not.
     *
     * \param targetService specifies the target of the subscription.
     *
     * \param source specifies the location where the subscription came.
     *
     * \param isNew indicates the type of the subscription. The value is
     * \e true in case the subscription is new and \e false in case the
     * subscription is a renewal to an existing subscription.
     *
     * \return \e true in case the subscription should be accepted.
     *
     * \remarks by default all subscriptions are accepted.
     */
    virtual bool acceptSubscription(
        HService* targetService, const HEndpoint& source, bool isNew);

protected:

    HDeviceHostPrivate* h_ptr;

    /*!
     * Returns the configuration used to initialize the device host.
     *
     * \return the configuration used to initialize the device host or null
     * in case the device host is not initialized.
     *
     * \remarks the returned object is not a copy and the ownership of the
     * object is not transferred.
     */
    const HDeviceHostConfiguration* configuration() const;

public:

    /*!
     * Creates a new instance.
     *
     * \param parent specifies the parent \c QObject.
     */
    explicit HDeviceHost(QObject* parent = 0);

    /*!
     * Destroys the device host and every hosted device.
     */
    virtual ~HDeviceHost();

    /*!
     * Initializes the device host and the devices it is supposed to host.
     *
     * \param configuration specifies the configuration for the instance. The
     * object has to contain at least one device configuration.
     *
     * \param errorString will contain a textual error description
     * in case the call failed and a pointer to a valid \c QString object was specified by the caller.
     *
     * \retval HDeviceHost::Success when the host was successfully started.
     *
     * \retval HDeviceHost::AlreadyInitialized when the host has already been successfully started.
     *
     * \retval HDeviceHost::InvalidConfiguration when the provided initialization parameters
     * contained one or more erroneous values, such as missing an \c HDeviceCreator.
     *
     * \retval HDeviceHost::InvalidDeviceDescription when the provided device description file
     * is invalid.
     *
     * \retval HDeviceHost::InvalidServiceDescription when the provided service description file
     * is invalid.
     *
     * \retval HDeviceHost::UndefinedFailure in case some other initialization error occurred.
     */
    ReturnCode init(
        const Herqq::Upnp::HDeviceHostConfiguration& configuration,
        QString* errorString = 0);

    /*!
     * Indicates whether or not the host is successfully started.
     *
     * \return \e true in case the host is successfully started.
     */
    bool isStarted() const;

    /*!
     * Returns a list of UPnP root devices the host is currently managing.
     *
     * The returned list contains pointers to root HDevice objects that are currently
     * hosted by this instance.
     *
     * \return a list of pointers to HDevice objects that are currently managed
     * by the device host.
     *
     * \warning the returned HDevice instances will be deleted when the
     * device host is being destroyed. However, do not delete
     * the device objects directly. The ownership of an HDevice is \b never transferred.
     */
    HDeviceList rootDevices() const;

    /*!
     * Returns a root device with the specified Unique Device Name.
     *
     * \param udn specifies the Unique Device Name of the desired root device.
     *
     * \return the root device with the specified Unique Device Name or a
     * null pointer in case no currently managed root device has the
     * specified UDN.
     *
     * \warning the returned device will be deleted when the
     * device host is being destroyed. However, do not delete
     * the device object directly. The ownership of an HDevice is \b never transferred.
     */
    HDevice* rootDevice(const HUdn& udn) const;

public Q_SLOTS:

    /*!
     * Quits the device host and destroys the UPnP devices it is hosting. Note that
     * this is automatically called when the device host is destroyed.
     *
     * \attention Every pointer to object retrieved from this instance will be
     * deleted. Be sure not to use any such pointer after calling this method.
     *
     * \warning This method usually returns promptly, but in some scenarios it
     * may involve invoking the \c QAbstractEventDispatcher to process events.
     * Be sure not to call init() before this method has properly exited.
     *
     * \sa init()
     */
    void quit();
};

}
}

#endif /* HDEVICEHOST_H_ */
