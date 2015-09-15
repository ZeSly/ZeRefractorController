/****************************************************************************
** Meta object code from reading C++ file 'firmwareupdate.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../firmwareupdate.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'firmwareupdate.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_FirmwareUpdate_t {
    QByteArrayData data[15];
    char stringdata[210];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FirmwareUpdate_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FirmwareUpdate_t qt_meta_stringdata_FirmwareUpdate = {
    {
QT_MOC_LITERAL(0, 0, 14),
QT_MOC_LITERAL(1, 15, 21),
QT_MOC_LITERAL(2, 37, 0),
QT_MOC_LITERAL(3, 38, 3),
QT_MOC_LITERAL(4, 42, 15),
QT_MOC_LITERAL(5, 58, 4),
QT_MOC_LITERAL(6, 63, 19),
QT_MOC_LITERAL(7, 83, 12),
QT_MOC_LITERAL(8, 96, 14),
QT_MOC_LITERAL(9, 111, 8),
QT_MOC_LITERAL(10, 120, 10),
QT_MOC_LITERAL(11, 131, 20),
QT_MOC_LITERAL(12, 152, 17),
QT_MOC_LITERAL(13, 170, 21),
QT_MOC_LITERAL(14, 192, 17)
    },
    "FirmwareUpdate\0IoWithDeviceCompleted\0"
    "\0msg\0Comm::ErrorCode\0time\0IoWithDeviceStarted\0"
    "AppendString\0SetProgressBar\0newValue\0"
    "Connection\0IoWithDeviceComplete\0"
    "IoWithDeviceStart\0AppendStringToTextbox\0"
    "UpdateProgressBar"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FirmwareUpdate[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   59,    2, 0x06 /* Public */,
       6,    1,   66,    2, 0x06 /* Public */,
       7,    1,   69,    2, 0x06 /* Public */,
       8,    1,   72,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    0,   75,    2, 0x0a /* Public */,
      11,    3,   76,    2, 0x0a /* Public */,
      12,    1,   83,    2, 0x0a /* Public */,
      13,    1,   86,    2, 0x0a /* Public */,
      14,    1,   89,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, 0x80000000 | 4, QMetaType::Double,    3,    2,    5,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::Int,    9,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 4, QMetaType::Double,    3,    2,    5,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::Int,    9,

       0        // eod
};

void FirmwareUpdate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FirmwareUpdate *_t = static_cast<FirmwareUpdate *>(_o);
        switch (_id) {
        case 0: _t->IoWithDeviceCompleted((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< Comm::ErrorCode(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3]))); break;
        case 1: _t->IoWithDeviceStarted((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->AppendString((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->SetProgressBar((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->Connection(); break;
        case 5: _t->IoWithDeviceComplete((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< Comm::ErrorCode(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3]))); break;
        case 6: _t->IoWithDeviceStart((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 7: _t->AppendStringToTextbox((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 8: _t->UpdateProgressBar((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (FirmwareUpdate::*_t)(QString , Comm::ErrorCode , double );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&FirmwareUpdate::IoWithDeviceCompleted)) {
                *result = 0;
            }
        }
        {
            typedef void (FirmwareUpdate::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&FirmwareUpdate::IoWithDeviceStarted)) {
                *result = 1;
            }
        }
        {
            typedef void (FirmwareUpdate::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&FirmwareUpdate::AppendString)) {
                *result = 2;
            }
        }
        {
            typedef void (FirmwareUpdate::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&FirmwareUpdate::SetProgressBar)) {
                *result = 3;
            }
        }
    }
}

const QMetaObject FirmwareUpdate::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_FirmwareUpdate.data,
      qt_meta_data_FirmwareUpdate,  qt_static_metacall, 0, 0}
};


const QMetaObject *FirmwareUpdate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FirmwareUpdate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FirmwareUpdate.stringdata))
        return static_cast<void*>(const_cast< FirmwareUpdate*>(this));
    return QDialog::qt_metacast(_clname);
}

int FirmwareUpdate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void FirmwareUpdate::IoWithDeviceCompleted(QString _t1, Comm::ErrorCode _t2, double _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void FirmwareUpdate::IoWithDeviceStarted(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void FirmwareUpdate::AppendString(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void FirmwareUpdate::SetProgressBar(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
