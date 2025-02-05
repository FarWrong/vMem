savedcmd_/home/eg/vMem/guest_vmem/m_scan.mod := printf '%s\n'   m_scan.o | awk '!x[$$0]++ { print("/home/eg/vMem/guest_vmem/"$$0) }' > /home/eg/vMem/guest_vmem/m_scan.mod
