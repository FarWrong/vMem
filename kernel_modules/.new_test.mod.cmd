savedcmd_/home/edward/Documents/vMem/kernel_modules/new_test.mod := printf '%s\n'   new_test.o | awk '!x[$$0]++ { print("/home/edward/Documents/vMem/kernel_modules/"$$0) }' > /home/edward/Documents/vMem/kernel_modules/new_test.mod