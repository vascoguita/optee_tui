#!/bin/sh

cp $(dirname $0)/../manager/*.ta /lib/optee_armtz/.

$(dirname $0)/../setup/tui_setup 10.0.2.2 9000