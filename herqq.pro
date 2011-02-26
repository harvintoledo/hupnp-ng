TEMPLATE = subdirs
CONFIG  += ordered

exists(hupnp/src/hupnp_core/options.pri) {
    win32 {
        system(type nul > hupnp/src/hupnp_core/options.pri)
    }
    else {
        system(echo "" > hupnp/src/hupnp_core/options.pri)
    }
}

CONFIG(DISABLE_QTSOAP) {
    system(echo "CONFIG += DISABLE_QTSOAP" > hupnp/src/hupnp_core/options.pri)
}
CONFIG(USE_QT_INSTALL_LOC) {
    system(echo "CONFIG += USE_QT_INSTALL_LOC" >> hupnp/src/hupnp_core/options.pri)
}

!CONFIG(DISABLE_QTSOAP) : SUBDIRS += hupnp/lib/qtsoap-2.7-opensource/buildlib
!CONFIG(DISABLE_CORE) : SUBDIRS += hupnp/src/hupnp_core
!CONFIG(DISABLE_TESTAPP) : SUBDIRS += apps/simple_test-app
