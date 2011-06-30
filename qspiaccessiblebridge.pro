#version check qt
contains(QT_VERSION, ^4\\.[0-6]\\..*) {
message("Cannot build Qt at-spi bridge with Qt version $${QT_VERSION}.")
error("Use Qt 4.8.")
}

contains(QT_VERSION, ^4\\.7\\..*) {
message("Building Qt at-spi bridge with Qt version $${QT_VERSION} is not recommended.")
message("The bridge will not work well and most likely crash your application.")
warning("Use Qt 4.8.")
}

TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS += src \
           tests \
           examples/calculator \
           examples/declarative \
           examples/simple

