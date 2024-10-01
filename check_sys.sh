#!/bin/bash
#set -x

BOLD_GREEN='\033[1;32m'
BOLD_RED='\033[1;31m'
RESET='\033[0m'

start_check() {
    printf ".. Checking %-40s" "$1"
    errs=0
}

print_err() {
    if [[ $errs -eq "0" ]]; then
        echo -e " ${BOLD_RED}[FAIL]${RESET}"
    fi
    echo -e "\t L__ $1"
    ((errs=errs+1))
}

end_check() {
    if [[ $errs -eq "0" ]]; then
        echo -e " ${BOLD_GREEN}[OK]${RESET}"
    fi
}

extract_quoted() {
    # matches lines containing the argument and returns the substring
    # immediately following the argument until the first double quote
    sed -n -e 's/^.*'"$1"'\([^"]*\)".*/\1/p'
}

assert_contains() {
    if [[ ! $1 =~ $2 ]]; then
      print_err "\`$2\` $3"
    fi
}

assert_not_contains() {
    if [[ $1 =~ $2 ]]; then
      print_err "\`$2\` $3"
    fi
}

assert_not_contains_from_version() {
    if [[ "$(printf '%s\n' "$1" "$(uname -r)" | sort -V | head -n1)" == "$1" ]]; then
        if [[ $2 =~ $3 ]]; then
          print_err "\`$3\` $4"
        fi
    fi
}

############################################################################
start_check "recommended SGX-Step parameters"
sgxstep_cmdline=`cat README.md | extract_quoted "quiet splash "`
kernel_cmdline=`cat /proc/cmdline`
config_x2apic=$(grep -E '#define\s+X2APIC\s+[01]' libsgxstep/config.h | awk '{print $3}')

for c in $sgxstep_cmdline
do
    assert_contains "$kernel_cmdline" $c "not in /proc/cmdline"
done

if [ "$config_x2apic" -eq 0 ]; then
    assert_contains "$kernel_cmdline" "nox2apic" "not in /proc/cmdline, but config.h defines X2APIC=0"
else
    assert_not_contains "$kernel_cmdline" "nox2apic" "in /proc/cmdline, but config.h defines X2APIC=1"
fi
end_check

############################################################################
start_check "unknown kernel parameters"
dmesg_output=$(dmesg 2>&1)
unknown_cmdline=`echo "$dmesg_output" | extract_quoted 'Unknown kernel command line parameters "'`

assert_not_contains "$dmesg_output" "Operation not permitted" ": run this script as root"
for c in $unknown_cmdline
do
    # NOTE: pti=off wrongly reported as unknown by Linux kernel < 6.7
    if [[ $c =~ "pti" ]]; then
        assert_not_contains_from_version "6.7.0" "$sgxstep_cmdline" $c "reported unknown to kernel"
    else
        assert_not_contains "$sgxstep_cmdline" $c "reported unknown to kernel"
    fi
done
end_check

############################################################################
start_check "CPU features"
cpuinfo=`cat /proc/cpuinfo`
assert_not_contains "$cpuinfo" "smap"   "not disabled in /proc/cpuinfo"
assert_not_contains "$cpuinfo" "smep"   "not disabled in /proc/cpuinfo"
assert_not_contains "$cpuinfo" "umip"   "not disabled in /proc/cpuinfo"

if [ "$config_x2apic" -eq 0 ]; then
    assert_not_contains "$cpuinfo" "x2apic" "not disabled in /proc/cpuinfo, but config.h defines X2APIC=0"
else
    assert_contains "$cpuinfo" "x2apic" "not enabled in /proc/cpuinfo, but config.h defines X2APIC=1"
fi
end_check

############################################################################
start_check "kernel page-table isolation"
if [[ $cpuinfo =~ "cpu_meltdown" ]]; then
    kpti_status=`dmesg | grep "page tables isolation"`
    assert_contains  "$kpti_status" "disabled" "not found for KPTI"
fi
end_check

############################################################################
start_check "SGX-Step driver"
lkms=`lsmod`
assert_contains "$lkms" "sgx_step" "kernel module not loaded; try \`cd kernel; make load\`"
end_check

