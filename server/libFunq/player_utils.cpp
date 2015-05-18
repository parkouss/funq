/*
Copyright: SCLE SFE
Contributor: Julien Pagès <j.parkouss@gmail.com>

This software is a computer program whose purpose is to test graphical
applications written with the QT framework (http://qt.digia.com/).

This software is governed by the CeCILL v2.1 license under French law and
abiding by the rules of distribution of free software.  You can  use,
modify and/ or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability.

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and,  more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL v2.1 license and that you accept its terms.
*/

#include "player_utils.h"
#include "objectpath.h"
#include <QMetaProperty>

using namespace ObjectPath;

void activate_focus(QWidget * w) {
    w->activateWindow();
    w->setFocus(Qt::MouseFocusReason);
}

void dump_properties(QObject * object, QtJson::JsonObject & out) {
    const QMetaObject * metaobject = object->metaObject();
    for (int i = 0; i < metaobject->propertyCount(); ++i) {
        QMetaProperty prop = metaobject->property(i);
        QVariant value = object->property(prop.name());
        // first try to serialize and only add property if it is possible
        bool success = false;
        QtJson::serialize(value, success);
        if (success) {
            out[prop.name()] = value;
        }
    }
}

void dump_object(QObject * object, QtJson::JsonObject & out, bool with_properties) {
    out["path"] = objectPath(object);
    QStringList classes;
    const QMetaObject * mo = object->metaObject();
    while (mo) {
        // sometimes classes appears twice
        if (!classes.contains(mo->className())) {
            classes << mo->className();
        }
        mo = mo->superClass();
    }
    out["classes"] = classes;
    if (with_properties) {
        QtJson::JsonObject properties;
        dump_properties(object, properties);
        out["properties"] = properties;
    }
}

/* Context locator */

ObjectLocatorContext::ObjectLocatorContext(Player * player,
                                           const QtJson::JsonObject & command,
                                           const QString & oidKey) {
    id = command[oidKey].value<qulonglong>();
    obj = player->registeredObject(id);
    if (!obj) {
        lastError = player->createError("NotRegisteredObject",
                           QString::fromUtf8("The object (id:%1) is not registered or has been destroyed").arg(id));
    }
}
