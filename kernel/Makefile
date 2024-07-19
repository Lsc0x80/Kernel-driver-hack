# KDIR - Directory where locates kernel source
# Specify CROSS_COMPILE and ARCH flag here
OURMAKEFLAGS ?= O=out CC=clang LD=ld.lld AR=llvm-ar NM=llvm-nm STRIP=llvm-strip OBJCOPY=llvm-objcopy OBJDUMP=llvm-objdump READELF=llvm-readelf HOSTCC=clang HOSTCXX=clang++ HOSTAR=llvm-ar HOSTLD=ld.lld
KDIR ?= $(HOME)/kernels/k-crdroid
OUT_DIR := ./out

default:
	$(MAKE) -C $(KDIR) M=$(PWD) $(OURMAKEFLAGS)
	@mkdir -p $(OUT_DIR)/
	@mv -f src/*.o $(OUT_DIR)/
	@mv -f src/*.ko $(OUT_DIR)/
	@mv -f src/*.mod.c $(OUT_DIR)/
	@mv -f src/.*.cmd $(OUT_DIR)/
	@mv -f src/modules.order $(OUT_DIR)/
	
	@mv -f *.o $(OUT_DIR)/
	@mv -f Module.symvers $(OUT_DIR)/
	@mv -f modules.order $(OUT_DIR)/

clean:
	@rm -rf $(OUT_DIR)
