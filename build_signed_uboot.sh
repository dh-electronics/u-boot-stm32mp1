#!/bin/bash

set -euxo pipefail

ARCH=${ARCH:-"arm"}
CROSS_COMPILE=${CROSS_COMPILE:-"arm-linux-gnueabihf-"}
DEFCONFIG=${DEFCONFIG:-"stm32mp15_dhcom_secure_defconfig"}
KEY_ALGO=${KEY_ALGO:-"rsa"}
KEY_DIR=${KEY_DIR:-"../keys"}
KEY_POSTFIX=${KEY_POSTFIX:-"-$(date +%F)"}

export ARCH CROSS_COMPILE

# Check required tools
check_command() {
    local cmd="${1}"
    if ! command -v "${cmd}" >/dev/null 2>&1; then
        echo "${cmd} not available!"
        exit 1
    fi
}
check_command fdtget
check_command fdtput
check_command make
check_command openssl

# Build everything: tools, u-boot and u-boot spl
# An existing .config is not overwritten
build_uboot() {
    [ -r .config ] || make ${DEFCONFIG}
    make all -j"$(nproc)"
}
build_uboot

# import .config to shell env
# replace $() with ${} beforehand
sed -E 's/\$\((.*)\)/\${\1}/g' .config > .config_env
# shellcheck disable=SC1091
. .config_env
rm .config_env

# Check KEY_ALGO and set exact required values
case "${KEY_ALGO}" in
rsa*) KEY_ALGO=rsa4096;;
prime256*|ecdsa*) KEY_ALGO=ecdsa256;;
*) echo "Unexpected KEY_ALGO=${KEY_ALGO}"; exit 1;;
esac

# Create key pairs if they doesn't exist already
mkdir -p "${KEY_DIR}"
for t in fsbl-img${KEY_POSTFIX} fsbl-cfg${KEY_POSTFIX} ssbl-img${KEY_POSTFIX} ssbl-cfg${KEY_POSTFIX}; do
    if [ ${KEY_ALGO} = rsa4096 ] && [ ! -r "${KEY_DIR}"/"${t}".key ]; then
        openssl genpkey -algorithm RSA -out "${KEY_DIR}"/"${t}".key -pkeyopt rsa_keygen_bits:4096
        openssl req -batch -new -x509 -key "${KEY_DIR}"/"${t}".key -out "${KEY_DIR}"/"${t}".crt
    elif [ ${KEY_ALGO} = ecdsa256 ] && [ ! -r "${KEY_DIR}"/"${t}".pem ]; then
        openssl ecparam -genkey -name prime256v1 -noout -out "${KEY_DIR}"/"${t}".pem
        openssl req -batch -new -x509 -key "${KEY_DIR}"/"${t}".pem -out "${KEY_DIR}"/"${t}".crt
    fi
done

# Copy fit image templates if not already in source root and fill current key algo and name's
[ -r u-boot.its ] || cp "${CONFIG_SPL_FIT_SOURCE:-"board/dhelectronics/dh_stm32mp1/u-boot-dhcom.its"}" u-boot.its
[ -r linux.its ] || cp board/dhelectronics/dh_stm32mp1/linux-dhcom.its linux.its
sed -i "s/KEY_ALGO\|rsa[0-9]*\|ecdsa[0-9]*/${KEY_ALGO}/g" u-boot.its linux.its
sed -i "s/KEY_NAME_FSBL_IMG\|fsbl-img[-_a-zA-Z0-9]*/fsbl-img${KEY_POSTFIX}/g" u-boot.its
sed -i "s/KEY_NAME_FSBL_CFG\|fsbl-cfg[-_a-zA-Z0-9]*/fsbl-cfg${KEY_POSTFIX}/g" u-boot.its
sed -i "s/KEY_NAME_SSBL_IMG\|ssbl-img[-_a-zA-Z0-9]*/ssbl-img${KEY_POSTFIX}/g" linux.its
sed -i "s/KEY_NAME_SSBL_CFG\|ssbl-cfg[-_a-zA-Z0-9]*/ssbl-cfg${KEY_POSTFIX}/g" linux.its

create_clean_signature_node() {
    fdtput -r "${1}" /signature >/dev/null 2>&1 || true
    fdtput -c "${1}" /signature
}

# Create signed linux fit image and add public keys to dtb's
# mkimage supports only one dtb at a time for the parameter K
for dtb in arch/"${ARCH}"/dts/stm32mp15xx-dhco?*.dtb; do
    create_clean_signature_node "${dtb}"
    ./tools/mkimage -f linux.its -r -k "${KEY_DIR}" -K "${dtb}" linux-signed.itb
done

# Create signed u-boot fit image, add public key to dtb and add
# u-boot,dm-spl property into dtb to not drop public keys in build
cp dts/dt.dtb dts/dt-u-boot-spl.dtb
create_clean_signature_node dts/dt-u-boot-spl.dtb
./tools/mkimage -f u-boot.its -r -k "${KEY_DIR}" -K dts/dt-u-boot-spl.dtb u-boot-signed.itb
for node in $(fdtget -l dts/dt-u-boot-spl.dtb /signature); do
    fdtput dts/dt-u-boot-spl.dtb /signature/"${node}" u-boot,dm-spl
done

# Rebuild everything with custom dtb with added public key(s)
EXT_DTB=dts/dt-u-boot-spl.dtb
export EXT_DTB
build_uboot

# Rebuild signed fit images
./tools/mkimage -f linux.its -k "${KEY_DIR}" linux-signed.itb
./tools/mkimage -f u-boot.its -k "${KEY_DIR}" u-boot-signed.itb
