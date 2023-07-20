#!/bin/bash
#set -x

start_check() {
    printf ".. Checking %-40s" "$1"
    errs=0
}

print_err() {
    if [[ $errs -eq "0" ]]; then
        echo " [FAIL]"
    fi
    echo -e "\t L__ $1"
    ((errs=errs+1))
}

end_check() {
    if [[ $errs -eq "0" ]]; then
        echo " [OK]"
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

############################################################################
start_check "recommended SGX-Step parameters"
sgxstep_cmdline=`cat README.md | extract_quoted "quiet splash "`
kernel_cmdline=`cat /proc/cmdline`

for c in $sgxstep_cmdline
do
    assert_contains "$kernel_cmdline" $c "not in /proc/cmdline"
done
end_check

############################################################################
start_check "unknown kernel parameters"
unknown_cmdline=`dmesg | extract_quoted 'Unknown kernel command line parameters "'`

for c in $unknown_cmdline
do
    # XXX pti=off seems to be wrongly reported as unknown by Linux(?)
    if [[ ! $c =~ "pti" ]]; then
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
assert_not_contains "$cpuinfo" "x2apic" "not disabled in /proc/cpuinfo"
end_check

############################################################################
start_check "kernel page-table isolation"
if [[ $cpuinfo =~ "cpu_meltdown" ]]; then
    kpti_status=`dmesg | grep "page tables isolation"`
    assert_contains  "$kpti_status" "disabled" "not found for KPTI"
fi
end_check

