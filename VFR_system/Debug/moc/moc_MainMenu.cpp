/****************************************************************************
** Meta object code from reading C++ file 'MainMenu.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../MainMenu.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainMenu.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainMenu_t {
    QByteArrayData data[8];
    char stringdata0[131];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainMenu_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainMenu_t qt_meta_stringdata_MainMenu = {
    {
QT_MOC_LITERAL(0, 0, 8), // "MainMenu"
QT_MOC_LITERAL(1, 9, 27), // "DealSingleClickedVideoLabel"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 3), // "chn"
QT_MOC_LITERAL(4, 42, 27), // "DealDoubleClickedVideoLabel"
QT_MOC_LITERAL(5, 70, 20), // "DealAutoPlayAllVideo"
QT_MOC_LITERAL(6, 91, 20), // "DealStopPlayAllVideo"
QT_MOC_LITERAL(7, 112, 18) // "DealPlayVideoTimer"

    },
    "MainMenu\0DealSingleClickedVideoLabel\0"
    "\0chn\0DealDoubleClickedVideoLabel\0"
    "DealAutoPlayAllVideo\0DealStopPlayAllVideo\0"
    "DealPlayVideoTimer"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainMenu[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x0a /* Public */,
       4,    1,   42,    2, 0x0a /* Public */,
       5,    0,   45,    2, 0x0a /* Public */,
       6,    0,   46,    2, 0x0a /* Public */,
       7,    0,   47,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void MainMenu::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainMenu *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->DealSingleClickedVideoLabel((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->DealDoubleClickedVideoLabel((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->DealAutoPlayAllVideo(); break;
        case 3: _t->DealStopPlayAllVideo(); break;
        case 4: _t->DealPlayVideoTimer(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainMenu::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_MainMenu.data,
    qt_meta_data_MainMenu,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainMenu::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainMenu::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainMenu.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int MainMenu::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
