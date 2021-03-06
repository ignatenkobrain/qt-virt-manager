#include "dir_pool_stuff.h"

/*
 * http://libvirt.org/storage.html#StorageBackendDir
 */

Dir_Pool_Stuff::Dir_Pool_Stuff(QWidget *parent) :
    _Pool_Stuff(parent)
{
    source->setVisible(false);
}

/* public slots */
QDomDocument Dir_Pool_Stuff::getStorageXMLDesc() const
{
    QDomDocument doc;
    QDomElement _stuff, _target,
            _perm, _encrypt, _path;
    QDomText _text;
    _stuff = doc.createElement("stuff");
    doc.appendChild(_stuff);
    _target = doc.createElement("target");
    _stuff.appendChild(_target);
    _path = doc.createElement("path");
    _text = doc.createTextNode(target->path->text());
    _path.appendChild(_text);
    _target.appendChild(_path);
    if ( target->usePerm->isChecked() ) {
        QDomElement _owner, _group, _mode, _label;
        _perm = doc.createElement("permissions");
        _target.appendChild(_perm);
        _owner = doc.createElement("owner");
        _text = doc.createTextNode(target->owner->text());
        _owner.appendChild(_text);
        _perm.appendChild(_owner);
        _group = doc.createElement("group");
        _text = doc.createTextNode(target->group->text());
        _group.appendChild(_text);
        _perm.appendChild(_group);
        _mode = doc.createElement("mode");
        _text = doc.createTextNode(target->mode->text());
        _mode.appendChild(_text);
        _perm.appendChild(_mode);
        _label = doc.createElement("label");
        _text = doc.createTextNode(target->label->text());
        _label.appendChild(_text);
        _perm.appendChild(_label);
    };
    if ( target->encrypt->isUsed() ) {
        _encrypt = doc.createElement("encryption");
        _target.appendChild(_encrypt);
        _encrypt.setAttribute(
                    "format",
                    target->encrypt->getFormat());
    };
    return doc;
}
