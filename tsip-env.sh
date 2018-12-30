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
