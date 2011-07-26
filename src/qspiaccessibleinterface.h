#ifndef QSPIACCESSIBLEINTERFACE_H
#define QSPIACCESSIBLEINTERFACE_H

#include "struct_marshallers.h"

class QAccessibleInterface;
class QDBusConnection;
class QDBusMessage;
class QDBusVariant;
class QObject;
class QString;
class QVariant;

class QSpiAccessibleInterface
{
public:
    QSpiAccessibleInterface();

    bool handleMessage(QAccessibleInterface *interface, int child, const QString &function, const QDBusMessage &message, const QDBusConnection &connection);

private:
    void sendReply(const QDBusConnection &connection, const QDBusMessage &message, const QVariant &argument);
    void sendReply(const QDBusConnection &connection, const QDBusMessage &message, const QDBusVariant &argument);
    QAccessibleInterface *accessibleParent(QAccessibleInterface *iface, int child);


    static QString pathForInterface(QAccessibleInterface *interface, int index);
    static QString pathForObject(QObject *object);

    QSpiRelationArray relationSet(QAccessibleInterface *interface, int child, const QDBusConnection &connection) const;
};

#endif // QSPIACCESSIBLEINTERFACE_H
