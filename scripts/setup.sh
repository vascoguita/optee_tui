#!/bin/sh

xtest --install-ta $(dirname $0)/../manager

$(dirname $0)/../setup/tui_setup 10.0.2.2 9000