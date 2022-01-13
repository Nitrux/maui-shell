
set(caskpower_SRCS
    powermanagementengine.cpp
    powermanagementjob.cpp
    powermanagementservice.cpp
)

target_link_libraries(plasma_engine_powermanagement
    KF5::Solid
    KF5::IdleTime
    KF5::CoreAddons
    KF5::I18n
    KF5::Service
    Qt::DBus
)

