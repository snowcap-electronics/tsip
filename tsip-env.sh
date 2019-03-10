# BOARD, CONF_FILE and ZEPHYR_BASE environment variables are used by Zephyr

TSIP_APP="${1}"
BOARD="${2}"

CONF_FILE="prj.conf"

if [ -z "${TSIP_APP}" ] || [ -z "${BOARD}" ]
then
    echo "Usage: $0 app_name board"
elif [ ! -f tsip-env.sh ]
then
    echo "Please source this file on the top directory of the tsip source tree."
else
    rm prj.conf
    rm -f ${BOARD}.overlay

    if [ -f "configs/board-${BOARD}.conf" ]
    then
        echo "Using ${BOARD} specific conf"
        cat "configs/board-${BOARD}.conf" >> prj.conf
    fi

    if [ -f "configs/app-${TSIP_APP}.conf" ]
    then
        echo "Using ${TSIP_APP} specific conf"
        cat "configs/app-${TSIP_APP}.conf" >> prj.conf
    fi

    if [ -f "configs/app-${TSIP_APP}-${BOARD}.conf" ]
    then
        echo "Using ${TSIP_APP} and ${BOARD} specific conf"
        cat "configs/app-${TSIP_APP}-${BOARD}.conf" >> prj.conf
    fi

    echo "Applying generic conf"
    cat configs/generic.conf >> prj.conf

    echo "Applying debug conf"
    cat configs/debug.conf >> prj.conf

    if [ -f "configs/${TSIP_APP}-${BOARD}.overlay" ]
    then
        echo "Using ${TSIP_APP} and ${BOARD} specific device tree overlay"
        # Using BOARD.overlay here as Zephyr includes that automatically
        cat "configs/${TSIP_APP}-${BOARD}.overlay" >> ${BOARD}.overlay
    fi

    TSIP_TOP=$(pwd)
    export ZEPHYR_BASE="${TSIP_TOP}"/zephyr/
    if [ -d "${ZEPHYR_BASE}" ]
    then
        . $ZEPHYR_BASE/zephyr-env.sh
    else
        echo "FATAL: Zephyr directory not found, failed to set up Zephyr environment!"
    fi

    if [ ! -f "${CONF_FILE}" ]
    then
        echo "WARNING: Failed to find the config file '${CONF_FILE}'"
    fi

    if ! find "${ZEPHYR_BASE}/boards" | grep -q "${BOARD}"
    then
        echo "WARNING: Failed to find the board '${BOARD}'"
    fi
    echo "Setting up environment for application '${TSIP_APP}' using board '${BOARD}' and config '${CONF_FILE}'."

	export TSIP_APP BOARD CONF_FILE
fi
