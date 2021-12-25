#!/bin/sh

#FIXME Add environment variables without requiring systemd components.

for ENV_VAR in DESKTOP_SESSION \
               DISPLAY \
               XAUTHORITY \
               SESSION_MANAGER \
               XDG_CONFIG_DIRS \
               XDG_DATA_DIRS \
               XDG_RUNTIME_DIR \
               XDG_SEAT_PATH \
               XDG_SEAT \
               XDG_SESSION_CLASS \
               XDG_SESSION_ID \
               XDG_SESSION_PATH \
               XDG_SESSION_TYPE \
               XDG_VTNR;
do
    systemctl --user import-environment ${ENV_VAR}
done

# Launch a session bus instance
if [ -z "$DBUS_SESSION_BUS_ADDRESS" ] && type dbus-launch >/dev/null; then
    eval `dbus-launch --auto-syntax --exit-with-session`
fi

# Check whether D-Bus is OK
if ! qdbus >/dev/null 2>/dev/null; then
    echo "Could not start D-Bus session bus" 1>&2
    exit 1
fi

# Start the actual session
exec cask
