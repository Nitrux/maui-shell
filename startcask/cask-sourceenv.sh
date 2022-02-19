for i in $@
do
    . $i >/dev/null
done

# env may not support -0, fall back to GNU env
env -0 2>/dev/null || genv -0
