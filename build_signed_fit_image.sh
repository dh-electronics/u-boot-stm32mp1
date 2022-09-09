#!/bin/bash

set -euxo pipefail

KEY_ALGO=${KEY_ALGO:-"rsa4096"}
KEY_DIR=${KEY_DIR:-"../keys"}
ITS_FILE=${ITS_FILE:-${1:-"linux.its"}}
ITB_FILE=${ITS_FILE/.*/}-signed.itb

get_last_ssbl_key_name() {
    local type=${1:-"img"}
    local tmp=""
    # shellcheck disable=SC2012
    tmp=$(ls -1t "${KEY_DIR}"/ssbl-"${type}"*.key | head -n 1)
    tmp=${tmp##*/}
    tmp=${tmp/.key/}
    echo "${tmp}"
}

# Use by default the last created keys
KEY_NAME_SSBL_IMG=${KEY_NAME_SSBL_IMG:-$(get_last_ssbl_key_name img)}
KEY_NAME_SSBL_CFG=${KEY_NAME_SSBL_CFG:-$(get_last_ssbl_key_name cfg)}

# Update key algorithms in its file
if [[ "${KEY_ALGO}" = rsa4096 ]] || [[ "${KEY_ALGO}" = ecdsa256 ]]; then
    sed -i "s/KEY_ALGO\|rsa[0-9]*\|ecdsa[0-9]*/${KEY_ALGO}/g" "${ITS_FILE}"
else
    echo "!!! Failed to update key algorithms in ${ITS_FILE} !!!"
fi

# Update key names in its file
if [[ "${KEY_NAME_SSBL_IMG}" =  ssbl-img* ]] && [[ "${KEY_NAME_SSBL_CFG}" =  ssbl-cfg* ]]; then
    sed -i "s/KEY_NAME_SSBL_IMG\|ssbl-img[-_a-zA-Z0-9]*/${KEY_NAME_SSBL_IMG}/g" "${ITS_FILE}"
    sed -i "s/KEY_NAME_SSBL_CFG\|ssbl-cfg[-_a-zA-Z0-9]*/${KEY_NAME_SSBL_CFG}/g" "${ITS_FILE}"
else
    echo "!!! Failed to update key names in ${ITS_FILE} !!!"
fi

./tools/mkimage -f "${ITS_FILE}" -k "${KEY_DIR}" "${ITB_FILE}"

ls -la "${ITB_FILE}"
